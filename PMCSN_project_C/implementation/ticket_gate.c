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

double get_user_arrival_to_ticket_gate(double arrival)
{
	// TODO: bisogna capire come riunire i flussi
	//		e aggiungere la probabilità
	return (arrival);
}

double get_ticket_gate_departure(double start)
{
	SelectStream(11);
	double departure = start + Exponential(SR_TICKET_GATE_OPERATOR);
	return departure;
}

double get_abandon_ticket_gate(double arrival)
{
	SelectStream(14);
	double abandon = arrival + Exponential(P_LEAVE_TICKET_GATE);
	return abandon;
}

void user_arrivals_ticket_gate(void)
{
	printf("user arrive to ticket gate\n");
}
void user_departure_ticket_gate(void)
{
	printf("user departure to ticket gate\n");
}
void abandon_ticket_gate(void)
{
	printf("user abandon to ticket gate\n");
}