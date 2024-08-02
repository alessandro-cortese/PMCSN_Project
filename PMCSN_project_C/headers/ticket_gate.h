#include "./data_structures/event_list.h"
#include "./data_structures/states.h"
#include "./data_structures/area.h"
#include "./data_structures/time.h"
#include "./data_structures/loss.h"
#include "./data_structures/utils.h"

double get_user_arrival_to_ticket_gate(double arrival);
double get_ticket_gate_departure(double start);
double get_abandon_ticket_gate(double arrival);
void user_arrivals_ticket_gate(void);
void user_departure_ticket_gate(void);
void abandon_ticket_gate(void);