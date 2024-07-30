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

struct event_list events;
struct states state[5];
struct area areas[5];
struct time *t;
struct loss loss[5];

double rate;

double get_user_arrival_to_ticket_machine(double arrival)
{
	SelectStream(1);
	arrival += Exponential(rate * (P_TICKET_PURCHASED_FROM_TICKET_STATION * P_TICKET_NOT_PURCHASED));
	return (arrival);
}

double get_user_arrival_to_ticket_office(double arrival)
{
	SelectStream(2);
	arrival += Exponential(rate * P_TICKET_PURCHASED_FROM_TICKET_OFFICE * P_TICKET_NOT_PURCHASED);
	return (arrival);
}

double get_ticket_machine_departure(double start)
{
	SelectStream(3);
	double departure = start + Exponential(SR_TICKET_STATION);
	return departure;
}

double get_ticket_office_departure(double start)
{
	SelectStream(4);
	double departure = start + Exponential(SR_TICKET_OFFICE_OPERATOR);
	return departure;
}

double get_customer_support_departure(double start)
{
	SelectStream(5);
	double departure = start + Exponential(SR_CUSTOMER_SUPPORT_OPERATOR);
	return departure;
}

double get_security_check_departure(double start)
{
	SelectStream(6);
	double departure = start + Exponential(SR_SECURITY_CONTROL_OPERATOR);
	return departure;
}

double get_ticket_gate_departure(double start)
{
	SelectStream(7);
	double departure = start + Exponential(SR_TICKET_GATE_OPERATOR);
	return departure;
}

double get_abandon_ticket_machine(double arrival)
{
	SelectStream(8);
	double abandon = arrival + Exponential(P_LEAVE_TICKET_STATION);
	return abandon;
}

double get_abandon_ticket_office(double arrival)
{
	SelectStream(9);
	double abandon = arrival + Exponential(P_LEAVE_TICKET_OFFICE);
	return abandon;
}

double get_abandon_customer_support(double arrival)
{
	SelectStream(10);
	double abandon = arrival + Exponential(P_LEAVE_CUSTOMER_SUPPORT);
	return abandon;
}

double get_abandon_security_check(double arrival)
{
	SelectStream(11);
	double abandon = arrival + Exponential(P_LEAVE_SECURITY_CONTROL);
	return abandon;
}

double get_abandon_ticket_gate(double arrival)
{
	SelectStream(12);
	double abandon = arrival + Exponential(P_LEAVE_TICKET_GATE);
	return abandon;
}

void initializeEventList(int *n)
{
	// Init ticket machine
	events.user_arrival_to_ticket_machine.user_arrival_time = get_user_arrival_to_ticket_machine(START);
	printf("%f\n", events.user_arrival_to_ticket_machine.user_arrival_time);
	events.user_arrival_to_ticket_machine.is_user_arrival_active = true;
	// Init ticket office
	events.user_arrival_to_ticket_office.user_arrival_time = get_user_arrival_to_ticket_office(START);
	printf("%f\n", events.user_arrival_to_ticket_office.user_arrival_time);
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

	//
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
		   events.user_arrival_to_customer_support.is_user_arrival_active || events.user_arrival_to_security_check.is_user_arrival_active ||
		   events.user_arrival_to_ticket_gate.is_user_arrival_active || !is_system_empty(state, n))
	{

		t->next = get_minimum_time(events, state, n);
		calcultate_area_struct(n);

		struct next_abandon *next_ticket_machine_abandon = get_min_abandon(events.head_ticket_machine);
		struct next_abandon *next_ticket_office_abandon = get_min_abandon(events.head_ticket_office);
		struct next_abandon *next_customer_support_abandon = get_min_abandon(events.head_customer_support);
		struct next_abandon *next_security_check_abandon = get_min_abandon(events.head_security_check);
		struct next_abandon *next_ticket_ticket_gate_abandon = get_min_abandon(events.head_ticket_gate);

		t->current = t->next;
	}

	user_arrivals_ticket_machine();
	user_departure_ticket_machine();
	abandon_ticket_machine();

	user_arrivals_ticket_office();
	user_departure_ticket_office();
	abandon_ticket_office();

	user_arrivals_customer_support();
	user_departure_customer_support();
	abandon_customer_support();

	user_arrivals_security_check();
	user_departure_security_check();
	abandon_security_check();

	user_arrivals_ticket_gate();
	user_departure_ticket_gate();
	abandon_ticket_gate();

	return 0;
}