#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <stdbool.h>

struct queue_node
{
    int id;
    double arrival_time;
    struct queue_node *prev;
    struct queue_node *next;
};

struct user_arrival
{
    double user_arrival_time;
    bool is_user_arrival_active;
};

struct event_list
{
    // web_server
    struct user_arrival user_arrival_to_web_server;
    double *completionTimes_web_server;

    // spike server
    struct user_arrival user_arrival_to_spike_server;
    double *completion_times_spike_server;

    // queue for web_server
    struct queue_node *head_web_server;
    struct queue_node *tail_web_server;

    // queue for spike_server
    struct queue_node *head_spike_server;
    struct queue_node *tail_spike_server;
};

#endif