#include "../data_structures/event_list.h"
#include "../data_structures/states.h"
#include "../data_structures/area.h"
#include "../data_structures/time.h"
#include "../data_structures/loss.h"
#include "../data_structures/utils.h"
#include "../headers/ticket_gate.h"
#include "../headers/constants.h"
#include "../headers/rngs.h"
#include "../headers/utility_functions.h"

// double get_user_arrival_to_security_check(double arrival);
double get_security_check_departure(double start);
// double get_abandon_security_check(double arrival);
void user_arrivals_security_check(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate);
void user_departure_security_check(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset, bool infinite);