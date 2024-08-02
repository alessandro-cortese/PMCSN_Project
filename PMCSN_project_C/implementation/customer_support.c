#include <stdio.h>
#include "../headers/customer_support.h"

double get_user_arrival_to_customer_support(double arrival)
{
	// TODO: bisogna capire come riunire i flussi
	//		e aggiungere la probabilità
	return (arrival);
}

double get_customer_support_departure(double start)
{
	SelectStream(6);
	double departure = start + Exponential(SR_CUSTOMER_SUPPORT_OPERATOR);
	return departure;
}

double get_abandon_customer_support(double arrival)
{
	SelectStream(11);
	double abandon = arrival + Exponential(P_LEAVE_CUSTOMER_SUPPORT);
	return abandon;
}

void user_arrivals_customer_support(void)
{
	printf("user arrive to customer support\n");
}
void user_departure_customer_support(void)
{
	printf("user departure to customer support\n");
}
void abandon_customer_support(void)
{
	printf("user abandon to customer support\n");
}