#include <stdio.h>
#include "../headers/security_check.h"

double get_user_arrival_to_security_check(double arrival)
{
	// TODO: bisogna capire come riunire i flussi
	//		e aggiungere la probabilità
	return (arrival);
}

double get_security_check_departure(double start)
{
	SelectStream(7);
	double departure = start + Exponential(SR_SECURITY_CONTROL_OPERATOR);
	return departure;
}

double get_abandon_security_check(double arrival)
{
	SelectStream(12);
	double abandon = arrival + Exponential(P_LEAVE_SECURITY_CONTROL);
	return abandon;
}

void user_arrivals_security_check(void)
{
	printf("user arrive to sec check\n");
}
void user_departure_security_check(void)
{
	printf("user departure to security check\n");
}
void abandon_security_check(void)
{
	printf("user abandon to sec check\n");
}