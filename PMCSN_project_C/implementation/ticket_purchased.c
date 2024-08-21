#include "../data_structures/event_list.h"
#include "../data_structures/states.h"
#include "../data_structures/area.h"
#include "../data_structures/time.h"
#include "../data_structures/loss.h"
#include "../data_structures/utils.h"
#include "../headers/ticket_machine.h"
#include "../headers/ticket_office.h"
#include "../headers/customer_support.h"
#include "../headers/security_check.h"
#include "../headers/ticket_gate.h"
#include "../headers/constants.h"
#include "../headers/rngs.h"
#include "../headers/utility_functions.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int count_id = 0;

double get_user_arrival_to_ticket_purchased(double arrival, double rate)
{
    SelectStream(0);
    arrival += Exponential(rate / P_OF_TICKET_PURCHASED_ONLINE);
    return (arrival);
}

void append_user_arrival_ticket_purchased(struct event_list *events, struct time *time, double rate)
{
    count_id++;
    events->user_who_has_purchased_ticket.user_arrival_time = get_user_arrival_to_ticket_purchased(time->current, rate);
    if (events->user_who_has_purchased_ticket.user_arrival_time > STOP)
    {
        events->user_who_has_purchased_ticket.user_arrival_time = (double)INFINITY;
        events->user_who_has_purchased_ticket.is_user_arrival_active = false;
        printf("Stop arrival to ticket purchased!\n");
    }
    else
    {
        struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
        if (!tail_job)
        {
            printf("Error in malloc in append user arrival ticket purchased!\n");
            exit(-1);
        }
        tail_job->id = count_id;
        tail_job->arrival_time = time->current;

        if (events->head_ticket_purchased == NULL)
        {
            events->head_ticket_purchased = tail_job;
            events->head_ticket_purchased->prev = NULL;
            events->head_ticket_purchased->next = NULL;
            events->tail_ticket_purchased = tail_job;
        }
        else if (events->head_ticket_purchased != NULL)
        {
            events->tail_ticket_purchased->next = tail_job;
            tail_job->prev = events->tail_ticket_purchased;
            tail_job->next = NULL;
            events->tail_ticket_purchased = tail_job;
        }

        tail_job = NULL;
        routing_ticket_purchased(events, time, rate);
    }
}