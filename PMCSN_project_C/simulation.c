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
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

struct event_list events;
struct states state[5];
struct area areas[5];
struct time t;
struct loss loss[5];

double rate;

double Exponential(double m)
{
	return (-m * log(1.0 - Random()));
}

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

bool is_system_empty(int *n)
{

	// This if check if there are job in the various centers
	if (state[0].population > 0 || state[1].population > 0 || state[2].population > 0 || state[3].population > 0 || state[4].population > 0)
		return false;

	// This for checks if there are jobs in the various server, both in the centers and in the queues
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < n[j]; i++)
		{
			if (state[j].server_occupation[i] != 0)
				return false;
		}
	}
}

struct next_abandon get_min_abandon(struct user *head)
{

	struct next_abandon min;
	min.user_Id = head->id;
	min.abandonTime = head->abandonTime;

	struct user *current = head;

	while (current != NULL)
	{
		if (current->abandonTime < min.abandonTime)
		{
			min.user_Id = current->id;
			min.abandonTime = current->abandonTime;
		}
		current = current->next;
	}

	return min;
}

struct next_job get_min_service(int num_servers) {

	struct next_job min;
	min.serverOffset = 0;
	min.completionTime = (double) INFINITY;
	

}

int len(double *array)
{

	int count = 0;

	return count;
}

double get_smallest(double *values)
{

	double smallest = (double)INFINITY;

	for (int i = 0; i < DIM; i++)
	{
		if (values[i] < smallest)
			smallest = values[i];
	}

	return smallest;
}

double get_minimum_time(int *m)
{

	double min_abandon_ticket_machine = (double)INFINITY;
	double min_abandon_ticket_office = (double)INFINITY;
	double min_abandon_customer_support = (double)INFINITY;
	double min_abandon_security_check = (double)INFINITY;
	double min_abandon_ticket_gate = (double)INFINITY;

	if (events.head_ticket_machine != NULL)
	{
		min_abandon_ticket_machine = get_min_abandon(events.head_ticket_machine);
	}
	if (events.head_ticket_office != NULL)
	{
		min_abandon_ticket_office = get_min_abandon(events.head_ticket_office);
	}

	if (events.head_customer_support != NULL)
	{
		min_abandon_customer_support = get_min_abandon(events.head_customer_support);
	}

	if (events.head_security_check != NULL)
	{
		min_abandon_security_check = get_min_abandon(events.head_security_check);
	}

	if (events.head_ticket_gate != NULL)
	{
		min_abandon_ticket_gate = get_min_abandon(events.head_ticket_gate);
	}

	double min_service_ticket_machine = (double)INFINITY;
	double min_service_ticket_office = (double)INFINITY;
	double min_service_customer_support = (double)INFINITY;
	double min_service_security_check = (double)INFINITY;
	double min_service_ticket_gate = (double)INFINITY;

	for (int i = 0; i < m[0]; i++)
	{
		if (events.completionTimes_ticket_machine[i] < min_service_ticket_machine)
			min_service_ticket_machine = events.completionTimes_ticket_machine[i];
	}
	for (int i = 0; i < m[1]; i++)
	{
		if (events.completionTimes_ticket_office[i] < min_service_ticket_office)
			min_service_ticket_office = events.completionTimes_ticket_office[i];
	}
	for (int i = 0; i < m[2]; i++)
	{
		if (events.completionTimes_customer_support[i] < min_service_customer_support)
			min_service_customer_support = events.completionTimes_customer_support[i];
	}
	for (int i = 0; i < m[3]; i++)
	{
		if (events.completionTimes_security_check[i] < min_service_security_check)
			min_service_security_check = events.completionTimes_security_check[i];
	}
	for (int i = 0; i < m[4]; i++)
	{
		if (events.completionTimes_ticket_gate[i] < min_service_ticket_gate)
			min_service_ticket_gate = events.completionTimes_ticket_gate[i];
	}

	double timesToCompare[DIM];

	timesToCompare[0] = min_abandon_ticket_machine;
	timesToCompare[1] = min_abandon_ticket_office;
	timesToCompare[2] = min_abandon_customer_support;
	timesToCompare[3] = min_abandon_security_check;
	timesToCompare[4] = min_abandon_ticket_gate;
	timesToCompare[5] = min_service_ticket_machine;
	timesToCompare[6] = min_service_ticket_office;
	timesToCompare[7] = min_service_customer_support;
	timesToCompare[8] = min_service_security_check;
	timesToCompare[9] = min_service_ticket_gate;
	timesToCompare[10] = events.user_arrival_to_ticket_machine.user_arrival_time;
	timesToCompare[11] = events.user_arrival_to_ticket_office.user_arrival_time;
	timesToCompare[12] = events.user_arrival_to_customer_support.user_arrival_time;
	timesToCompare[13] = events.user_arrival_to_security_check.user_arrival_time;
	timesToCompare[14] = events.user_arrival_to_ticket_gate.user_arrival_time;

	return get_smallest(timesToCompare);
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
	t.current = 0.0;
	t.next = 0.0;
	for (int i = 0; i < 5; i++)
	{
		t.last[i] = 0.0;
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
		   events.user_arrival_to_ticket_gate.is_user_arrival_active || !is_system_empty(n))
	{

		t.next = get_minimum_time(n);
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