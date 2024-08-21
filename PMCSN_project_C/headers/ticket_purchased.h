#ifndef TICKET_PURCHASED_H
#define TICKET_PURCHASED_H

double get_user_arrival_to_ticket_purchased(double arrival, double rate);
void append_user_arrival_ticket_purchased(struct event_list *events, struct time *time, double rate);

#endif