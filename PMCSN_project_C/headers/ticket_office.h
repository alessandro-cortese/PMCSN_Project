#include "./data_structures/event_list.h"
#include "./data_structures/states.h"
#include "./data_structures/area.h"
#include "./data_structures/time.h"
#include "./data_structures/loss.h"
#include "./data_structures/utils.h"
#include "./headers/customer_support.h"
#include "./headers/security_check.h"
#include "./headers/ticket_gate.h"
#include "./headers/constants.h"
#include "./headers/rngs.h"
#include "./headers/utility_functions.h"

double get_user_arrival_to_ticket_office(double arrival, double rate);
double get_ticket_office_departure(double start);
double get_abandon_ticket_office(double arrival);

void user_arrivals_ticket_office(void);
void user_departure_ticket_office(void);
void abandon_ticket_office(void);