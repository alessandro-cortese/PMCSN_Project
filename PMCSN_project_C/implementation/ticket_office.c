#include <stdio.h>
#include "../headers/ticket_office.h"

double get_user_arrival_to_ticket_office(double arrival, double rate)
{
	SelectStream(2);
	arrival += Exponential(rate * P_TICKET_PURCHASED_FROM_TICKET_OFFICE * P_TICKET_NOT_PURCHASED);
	return (arrival);
}

double get_ticket_office_departure(double start)
{
	SelectStream(4);
	double departure = start + Exponential(SR_TICKET_OFFICE_OPERATOR);
	return departure;
}

double get_abandon_ticket_office(double arrival)
{
	SelectStream(9);
	double abandon = arrival + Exponential(P_LEAVE_TICKET_OFFICE);
	return abandon;
}

void user_arrivals_ticket_office(void)
{
	printf("user arrive to ticket office\n");
}
void user_departure_ticket_office(void)
{
	printf("user departure to ticket office\n");
}
void abandon_ticket_office(void)
{
	printf("user abandon to ticket office\n");
}