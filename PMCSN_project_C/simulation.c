#include "./data_structures/event_list.h"
#include "./data_structures/states.h"
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

struct event_list events;
struct states state[5];		
struct area areas[5];
struct time t[5];
struct loss los[5];

double rate;

double Exponential(double m)
{
	return (-m * log(1.0 - Random()));
}

double getUserArrival(double arrival)
{
	arrival += Exponential(rate * (P_TICKET_PURCHASED_FROM_TICKET_STATION * P_TICKET_NOT_PURCHASED));
	return (arrival);
}

double getUserArrival2(double arrival)
{
	arrival += Exponential(rate * P_TICKET_PURCHASED_FROM_TICKET_OFFICE * P_TICKET_NOT_PURCHASED);
	return (arrival);
}
void initializeEventList(double *n, int numCenters)
{
	// Init ticket machine
	events.user_arrival_to_ticket_machine.user_arrival_time = getUserArrival(START);
	printf("%f\n", events.user_arrival_to_ticket_machine.user_arrival_time);
	events.user_arrival_to_ticket_machine.is_user_arrival_active = true;
	// Init ticket office
	events.user_arrival_to_ticket_office.user_arrival_time = getUserArrival2(START);
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

	for (int i = 0; i < numCenters; i++)
	{
		printf("%f\n", n[i]);
	}
}

int main(int argc, char **argv)
{

	if (argc < 7)
		exit(-1);

	int interval = atoi(argv[6]);

	double *n = NULL;
	n = (double *)malloc(sizeof(double) * (argc - 1));
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

	initializeEventList(n, argc - 2);

	user_arrivals1();
	user_departure1();
	abandon1();

	return 0;
}