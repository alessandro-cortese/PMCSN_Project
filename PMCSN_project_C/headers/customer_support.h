#include "./data_structures/event_list.h"
#include "./data_structures/states.h"
#include "./data_structures/area.h"
#include "./data_structures/time.h"
#include "./data_structures/loss.h"
#include "./data_structures/utils.h"
#include "./headers/security_check.h"
#include "./headers/ticket_gate.h"
#include "./headers/constants.h"
#include "./headers/rngs.h"
#include "./headers/utility_functions.h"

double get_user_arrival_to_customer_support(double arrival);
double get_customer_support_departure(double start);
double get_abandon_customer_support(double arrival);
void user_arrivals_customer_support(void);
void user_departure_customer_support(void);
void abandon_customer_support(void);