#include "../data_structures/event_list.h"
#include "../data_structures/states.h"
#include "../data_structures/area.h"
#include "../data_structures/time.h"
#include "../data_structures/loss.h"
#include "../data_structures/utils.h"
#include "../headers/ticket_office.h"
#include "../headers/customer_support.h"
#include "../headers/security_check.h"
#include "../headers/ticket_gate.h"
#include "../headers/constants.h"
#include "../headers/rngs.h"
#include "../headers/utility_functions.h"

double get_user_arrival_to_ticket_machine(double arrival, double rate);
double get_ticket_machine_departure(double start);
// double get_abandon_ticket_machine(double arrival);
void user_arrivals_ticket_machine(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate);
void user_departure_ticket_machine(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset, double rate);
void abandon_ticket_machine(struct event_list *events, struct states *state, struct loss *loss, int job_id);
