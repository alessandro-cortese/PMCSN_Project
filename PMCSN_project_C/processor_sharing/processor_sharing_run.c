#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "./../data_structures/event_list.h"
#include "./../data_structures/states.h"
#include "./../data_structures/area.h"
#include "./../data_structures/time.h"
#include "./../headers/utility_functions.h"

struct event_list events;
struct states state[5];
struct area areas[5];
struct time *t;
double last_event;

void initializeTime()
{
    t = (struct time *)malloc(sizeof(struct time));
    t->current = 0.0;
    t->next = 0.0;
    for (int i = 0; i < 2; i++)
    {
        t->last[i] = 0.0;
    }
}
void initializeArea()
{

    for (int i = 0; i < 2; i++)
    {
        areas[i].node = 0.0;
        areas[i].queue = 0.0;
        areas[i].service = 0.0;
    }
}

void initializeStateVariables(int *m)
{
    for (int j = 0; j < 2; j++)
    {
        state[j].population = 0;
        state[j].server_count = 0;
        state[j].server_occupation = 0;
    }
}

int main(int argc, char **argv)
{

    initializeTime();
    initializeArea();
    // initialize_number_of_centers();
    // initializeStateVariables(number_of_centers);
    // initializeArrivalLoss();
    // initializeEventList(number_of_centers, rate);

    // printf("\n\nRUN NUMBER %d OF FINITE HORIZON SIMULATION!\n", repetition_index);

    while (events.user_arrival_to_web_server.is_user_arrival_active || events.user_arrival_to_spike_server.is_user_arrival_active || !is_system_empty(state))
    {
        t->next = get_minimum_time(events, state);

        // printf("----------\n");
        // printf("t->current:%f\n", t->current);
        // printf("t->next: %f\n\n\n", t->next);
        calcultate_area_struct(number_of_centers);
        // printf("----------\n");

        if (t->next == t->current)
        {
            printf("Current time is equal to next time...\n");
            exit(-1);
        }

        t->current = t->next;

        if (t->current == events.user_arrival_to_web_server.user_arrival_time)
        {
            // printf("*** Evento user_arrivals_ticket_machine ***\n");
            user_arrivals_web_server(&events, t, &state[0], &loss[0], rate, stop_value);
        }

        else if (t->current == events.user_arrival_to_spike_server.user_arrival_time)
        {
            // printf("*** Evento user_arrivals_ticket_office ***\n");
            user_arrivals_spike_server(&events, t, &state[1], &loss[1], rate, stop_value);
        }

        if (t->next == (double)INFINITY)
        {
            printf("Something went wrong with simulation...\n");
            exit(-1);
        }

        // consistency_check_population(&events);
        last_event = t->current;
    }

    return 0;
}