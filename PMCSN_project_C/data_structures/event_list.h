#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <stdbool.h>

struct user
// job
{
    int id;
    double abandonTime;
    struct user *prev;
    struct user *next;
};

struct user_arrival
{
    double user_arrival_time;
    bool is_user_arrival_active;
};

struct event_list
{
    // user arrival to train station that have already the ticket
    struct user_arrival user_who_has_purchased_ticket;

    // ticket machine
    struct user_arrival user_arrival_to_ticket_machine;
    double *completionTimes_ticket_machine;
    // abandon queue ticket machine
    struct user *head_ticket_machine;
    struct user *tail_ticket_machine;
    // ticket office
    struct user_arrival user_arrival_to_ticket_office;
    double *completionTimes_ticket_office;
    // abandon queue ticket office
    struct user *head_ticket_office;
    struct user *tail_ticket_office;

    // list of job ticket machine + ticket office
    struct user *head_ticket_purchased;
    struct user *tail_ticket_purchased;

    // customer support
    struct user_arrival user_arrival_to_customer_support;
    double *completionTimes_customer_support;
    // abandon queue customer support
    struct user *head_customer_support;
    struct user *tail_customer_support;
    // queue of job in customer support
    struct user *head_queue_customer_support;
    struct user *tail_queue_customer_support;

    // list of job to security check
    struct user *head_user_to_security_check;
    struct user *tail_user_to_security_check;

    // security_check
    struct user_arrival user_arrival_to_security_check;
    double *completionTimes_security_check;
    struct user *head_security_check;
    struct user *tail_security_check;

    // ticket_gate
    struct user_arrival user_arrival_to_ticket_gate;
    double *completionTimes_ticket_gate;
    struct user *head_ticket_gate;
    struct user *tail_ticket_gate;
};

#endif