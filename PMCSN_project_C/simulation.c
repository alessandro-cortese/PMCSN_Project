#include "./data_structures/event_list.h"
#include "./headers/ticket_machine.h"
#include "./headers/ticket_office.h"
#include "./headers/customer_support.h"
#include "./headers/security_check.h"
#include "./headers/ticket_gate.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(void){
	user_arrivals1();
	user_departure1();
	abandon1();
	user_arrivals2();
	user_departure2();
	abandon2();
	user_arrivals3();
	user_departure3();
	user_arrivals4();
	user_departure4();
	user_arrivals5();
	user_departure5();
	return 0;
}