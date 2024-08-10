#include "./data_structures/event_list.h"
#include "./data_structures/states.h"
#include "./data_structures/area.h"
#include "./data_structures/time.h"
#include "./data_structures/loss.h"
#include "./data_structures/utils.h"
#include "./headers/ticket_machine.h"
#include "./headers/ticket_office.h"
#include "./headers/customer_support.h"
#include "./headers/security_check.h"
#include "./headers/ticket_gate.h"
#include "./headers/constants.h"
#include "./headers/rngs.h"
#include "./headers/utility_functions.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct event_list events;
struct states state[5];
struct area areas[5];
struct time *t;
struct loss loss[5];

double get_user_arrival_to_ticket_purchased(double arrival, double rate)
{
	/*
	if (Random() <= P_OF_TICKET_PURCHASED_ONLINE)
	{
		printf("rand pick purchased online\n");
		SelectStream(15);
		arrival += Exponential(rate / P_OF_TICKET_PURCHASED_ONLINE);
		return (arrival);
	}
	else
	{
		if(arrival != 0.0)
			return arrival;
	}
	*/
	SelectStream(15);
	arrival += Exponential(rate / P_OF_TICKET_PURCHASED_ONLINE);
	return (arrival);
}

double rate;

void initializeEventList(int *n)
{
	events.user_who_has_purchased_ticket.user_arrival_time = get_user_arrival_to_ticket_purchased(START, rate);
	printf("purchased online time arrival %f\n", events.user_who_has_purchased_ticket.user_arrival_time);
	//  Init ticket machine
	events.user_arrival_to_ticket_machine.user_arrival_time = get_user_arrival_to_ticket_machine(START, rate);
	printf("ticket machine time arrival %f\n", events.user_arrival_to_ticket_machine.user_arrival_time);
	events.user_arrival_to_ticket_machine.is_user_arrival_active = true;
	// Init ticket office
	events.user_arrival_to_ticket_office.user_arrival_time = get_user_arrival_to_ticket_office(START, rate);
	printf("ticket office time arrival %f\n", events.user_arrival_to_ticket_office.user_arrival_time);
	events.user_arrival_to_ticket_office.is_user_arrival_active = true;
	// Init customer support
	events.user_arrival_to_customer_support.user_arrival_time = (double)INFINITY;
	events.user_arrival_to_customer_support.is_user_arrival_active = true;
	// Init security check
	events.user_arrival_to_security_check.user_arrival_time = (double)INFINITY;
	events.user_arrival_to_security_check.is_user_arrival_active = true;
	// Init ticket gate
	events.user_arrival_to_ticket_gate.user_arrival_time = (double)INFINITY;
	events.user_arrival_to_ticket_gate.is_user_arrival_active = true;

	events.head_ticket_machine = NULL;
	events.tail_ticket_machine = NULL;
	events.head_ticket_office = NULL;
	events.tail_ticket_office = NULL;

	events.completionTimes_ticket_machine = (double *)malloc(sizeof(double) * n[0]);
	events.completionTimes_ticket_office = (double *)malloc(sizeof(double) * n[1]);
	events.completionTimes_customer_support = (double *)malloc(sizeof(double) * n[2]);
	events.completionTimes_security_check = (double *)malloc(sizeof(double) * n[3]);
	events.completionTimes_ticket_gate = (double *)malloc(sizeof(double) * n[4]);

	for (int i = 0; i < n[0]; i++)
	{
		events.completionTimes_ticket_machine[i] = (double)INFINITY;
	}
	for (int i = 0; i < n[1]; i++)
	{
		events.completionTimes_ticket_office[i] = (double)INFINITY;
	}
	for (int i = 0; i < n[2]; i++)
	{
		events.completionTimes_customer_support[i] = (double)INFINITY;
	}
	for (int i = 0; i < n[3]; i++)
	{
		events.completionTimes_security_check[i] = (double)INFINITY;
	}
	for (int i = 0; i < n[4]; i++)
	{
		events.completionTimes_ticket_gate[i] = (double)INFINITY;
	}
}

void initializeTime()
{
	t = (struct time *)malloc(sizeof(struct time));
	t->current = 0.0;
	t->next = 0.0;
	for (int i = 0; i < 5; i++)
	{
		t->last[i] = 0.0;
	}
}

void initializeArea()
{

	for (int i = 0; i < 5; i++)
	{
		areas[i].node = 0.0;
		areas[i].queue = 0.0;
		areas[i].service = 0.0;
	}
}

void initializeStateVariables(int *m)
{
	for (int j = 0; j < 5; j++)
	{
		state[j].population = 0;
		state[j].server_occupation = (int *)malloc(sizeof(int) * m[j]);
		for (int i = 0; i < m[j]; i++)
		{
			state[j].server_occupation[i] = 0; // 0 = IDLE, 1 = BUSY
		}
	}
}

void initializeArrivalLoss()
{
	for (int i = 0; i < 5; i++)
	{
		loss[i].index_user = 0;
		loss[i].loss_user = 0;
	}
}

void calcultate_area_struct(int *n)
{
	int ticket_machine_busy_servers = get_total_busy_servers(n[0], state[0].server_occupation);
	areas[0].service += (t->next - t->current) * ticket_machine_busy_servers;
	areas[0].queue += (t->next - t->current) * (state[0].population);
	areas[0].node += (t->next - t->current) * (state[0].population - ticket_machine_busy_servers);

	int ticket_office_busy_servers = get_total_busy_servers(n[1], state[1].server_occupation);
	areas[1].service += (t->next - t->current) * ticket_office_busy_servers;
	areas[1].queue += (t->next - t->current) * (state[1].population);
	areas[1].node += (t->next - t->current) * (state[1].population - ticket_office_busy_servers);

	int customer_support_busy_servers = get_total_busy_servers(n[2], state[2].server_occupation);
	areas[2].service += (t->next - t->current) * customer_support_busy_servers;
	areas[2].queue += (t->next - t->current) * (state[2].population);
	areas[2].node += (t->next - t->current) * (state[2].population - customer_support_busy_servers);

	int security_check_busy_servers = get_total_busy_servers(n[3], state[3].server_occupation);
	areas[3].service += (t->next - t->current) * security_check_busy_servers;
	areas[3].node += (t->next - t->current) * (state[3].population - security_check_busy_servers);

	int ticket_gate_busy_servers = get_total_busy_servers(n[4], state[4].server_occupation);
	areas[4].service += (t->next - t->current) * ticket_gate_busy_servers;
	areas[4].queue += (t->next - t->current) * (state[4].population);
	areas[4].node += (t->next - t->current) * (state[4].population - ticket_gate_busy_servers);
}

void append_user_arrival_ticket_purchased()
{
	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in append user arrival ticket purchased!\n");
		exit(-1);
	}
	events.user_who_has_purchased_ticket.user_arrival_time = get_user_arrival_to_ticket_purchased(t->current, rate);
	if (events.user_who_has_purchased_ticket.user_arrival_time > STOP)
	{
		events.user_who_has_purchased_ticket.user_arrival_time = (double)INFINITY;
		events.user_who_has_purchased_ticket.is_user_arrival_active = false;
		printf("Stop arrival to ticket purchased!\n");
	}

	tail_job->id = loss->index_user;
	tail_job->arrival_time = t->current;

	if (events.head_ticket_purchased == NULL)
	{
		printf("Dentro, è la prima volta!\n");
		events.head_ticket_purchased = tail_job;
		events.head_ticket_purchased->prev = NULL;
		events.head_ticket_purchased->next = NULL;
		events.tail_ticket_purchased = tail_job;
	}
	else if (events.head_ticket_purchased != NULL)
	{
		events.tail_ticket_purchased->next = tail_job;
		tail_job->prev = events.tail_ticket_purchased;
		tail_job->next = NULL;
		events.tail_ticket_purchased = tail_job;
	}

	tail_job = NULL;
	routing_ticket_purchased(&events, t, state, loss, rate);
}

int main(int argc, char **argv)
{

	if (argc < 7)
		exit(-1);

	int interval = atoi(argv[6]);

	int *n = NULL;
	n = (int *)malloc(sizeof(int) * (argc - 1));
	if (n == NULL)
	{
		printf("Error in first malloc\n");
		exit(-2);
	}

	n[0] = atoi(argv[1]);
	n[1] = atoi(argv[2]);
	n[2] = atoi(argv[3]);
	n[3] = atoi(argv[4]);
	n[4] = atoi(argv[5]);

	switch (interval)
	{
	case 1:
		rate = LAMBDA1;
		break;

	case 2:
		rate = LAMBDA2;
		break;

	case 3:
		rate = LAMBDA3;
		break;

	default:
		exit(-3);
	}

	initializeTime();
	initializeArea();
	initializeStateVariables(n);
	initializeArrivalLoss();

	PlantSeeds(SEED);
	initializeEventList(n);

	while (events.user_arrival_to_ticket_machine.is_user_arrival_active || events.user_arrival_to_ticket_office.is_user_arrival_active ||
		   events.user_who_has_purchased_ticket.is_user_arrival_active || !is_system_empty(state, n))
	{
		t->next = get_minimum_time(events, state, n);
		printf("t->next: %f\n", t->next);
		printf("t->current:%f\n", t->current);
		calcultate_area_struct(n);

		struct next_abandon *next_ticket_machine_abandon = get_min_abandon(events.head_ticket_machine);
		struct next_abandon *next_ticket_office_abandon = get_min_abandon(events.head_ticket_office);
		struct next_abandon *next_customer_support_abandon = get_min_abandon(events.head_customer_support);

		struct next_job *next_job_ticket_machine = get_min_queue_time(events, n[0], state[0].server_occupation, 1);
		struct next_job *next_job_ticket_office = get_min_queue_time(events, n[1], state[1].server_occupation, 2);
		struct next_job *next_job_customer_support = get_min_queue_time(events, n[2], state[2].server_occupation, 3);
		struct next_job *next_job_security_check = get_min_queue_time(events, n[3], state[3].server_occupation, 4);
		struct next_job *next_job_ticket_gate = get_min_queue_time(events, n[4], state[4].server_occupation, 5);

		double min_job_completion_ticket_machine = next_job_ticket_machine->completionTime;
		double min_job_completion_ticket_office = next_job_ticket_office->completionTime;
		double min_job_completion_customer_support = next_job_customer_support->completionTime;
		double min_job_completion_security_check = next_job_security_check->completionTime;
		double min_job_completion_ticket_gate = next_job_ticket_gate->completionTime;

		t->current = t->next;

		if (t->current == events.user_who_has_purchased_ticket.user_arrival_time)
		{
			printf("Evento append_user_arrival_ticket_purchased\n");
			append_user_arrival_ticket_purchased();
			printf("Dopo l'evento di append\n");
		}
		else if (t->current == events.user_arrival_to_ticket_machine.user_arrival_time)
		{
			printf("Evento user_arrivals_ticket_machine\n");
			user_arrivals_ticket_machine(&events, t, &state[0], &loss[0], rate);
		}
		else if (t->current == min_job_completion_ticket_machine)
		{
			printf("Evento user_departure_ticket_machine\n");
			user_departure_ticket_machine(&events, t, &state[0], &loss[0], next_job_ticket_machine->serverOffset, rate);
		}
		else if (t->current == next_ticket_machine_abandon->abandonTime)
		{
			printf("Evento abandon_ticket_machine\n");
			abandon_ticket_machine(&events, &state[0], &loss[0], next_ticket_machine_abandon->user_Id);
		}
		else if (t->current == events.user_arrival_to_ticket_office.user_arrival_time)
		{
			printf("Evento user_arrivals_ticket_office\n");
			user_arrivals_ticket_office(&events, t, &state[1], &loss[1], rate);
		}
		else if (t->current == min_job_completion_ticket_office)
		{
			printf("Evento user_departure_ticket_office\n");
			user_departure_ticket_office(&events, t, &state[1], &loss[1], next_job_ticket_office->serverOffset, rate);
		}
		else if (t->current == next_ticket_office_abandon->abandonTime)
		{
			printf("Evento abandon_ticket_office\n");
			abandon_ticket_office(&events, &state[1], &loss[1], next_ticket_office_abandon->user_Id);
		}
		else if (t->current == events.user_arrival_to_customer_support.user_arrival_time)
		{
			printf("Evento user_arrivals_customer_support\n");
			user_arrivals_customer_support(&events, t, &state[2], &loss[2], rate);
		}
		else if (t->current == min_job_completion_customer_support)
		{
			printf("Evento : user_departure_customer_support\n");
			user_departure_customer_support(&events, t, &state[2], &loss[2], next_job_security_check->serverOffset, rate);
		}
		else if (t->current == next_customer_support_abandon->abandonTime)
		{
			printf("Evento : abandon_customer_support\n");
			abandon_customer_support(&events, &state[2], &loss[2], next_customer_support_abandon->user_Id);
		}
		else if (t->current == events.user_arrival_to_security_check.user_arrival_time)
		{
			printf("Evento : user_arrivals_security_check\n");
			user_arrivals_security_check(&events, t, &state[3], &loss[3], rate);
		}
		else if (t->current == events.user_arrival_to_ticket_gate.user_arrival_time)
		{
			printf("Evento: user_arrivals_ticket_gate\n");
			user_arrivals_ticket_gate(&events, t, &state[4], &loss[4], rate);
		}
		else if (t->current == min_job_completion_ticket_gate)
		{
			printf("Evento : user_departure_ticket_gate\n");
			user_departure_ticket_gate(&events, t, &state[4], &loss[4], next_job_ticket_gate->serverOffset);
		}

		free(next_ticket_machine_abandon);
		free(next_ticket_office_abandon);
		free(next_customer_support_abandon);

		free(next_job_ticket_machine);
		free(next_job_ticket_office);
		free(next_job_customer_support);
		free(next_job_security_check);
		free(next_job_ticket_gate);

		sleep(2);

		puts("");
		puts("");
		puts("");
		puts("");
	}

	return 0;
}