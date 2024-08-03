#include "./../data_structures/event_list.h"
#include "./../data_structures/states.h"
#include "./../data_structures/area.h"
#include "./../data_structures/time.h"
#include "./../data_structures/loss.h"
#include "./../data_structures/utils.h"
#include "./../headers/ticket_machine.h"
#include "./../headers/ticket_office.h"
#include "./../headers/customer_support.h"
#include "./../headers/security_check.h"
#include "./../headers/ticket_gate.h"
#include "./../headers/constants.h"
#include "./../headers/rngs.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

double Exponential(double m)
{
    return (-m * log(1.0 - Random()));
}

bool is_system_empty(struct states *state, int *n)
{

    // This if check if there are job in the various centers
    if (state[0].population > 0 || state[1].population > 0 || state[2].population > 0 || state[3].population > 0 || state[4].population > 0)
        return false;

    // This for checks if there are jobs in the various server, both in the centers and in the queues
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < n[j]; i++)
        {
            if (state[j].server_occupation[i] != 0)
                return false;
        }
    }
}

struct next_abandon *get_min_abandon(struct user *head)
{
    // GetMinAbandon
    struct next_abandon *min = (struct next_abandon *)malloc(sizeof(struct next_abandon *));

    if (head != NULL)
    {
        min->user_Id = head->id;
        min->abandonTime = head->abandonTime;
    }

    struct user *current = head;

    while (current != NULL)
    {
        if (current->abandonTime < min->abandonTime)
        {
            min->user_Id = current->id;
            min->abandonTime = current->abandonTime;
        }
        current = current->next;
    }

    return min;
}

struct next_job *get_min_queue_time(struct event_list events, int num_servers, int *server_occupation, int index)
{
    // GetMinCompletion
    struct next_job *min = (struct next_job *)malloc(sizeof(struct next_job));
    if (!min)
    {
        printf("Error in malloc in get min queue time!\n");
        exit(-1);
    }
    min->serverOffset = 0;
    min->completionTime = (double)INFINITY;
    double *completionTimes = NULL;

    switch (index)
    {
    case 1:
        completionTimes = events.completionTimes_ticket_machine;
        break;
    case 2:
        completionTimes = events.completionTimes_ticket_office;
        break;
    case 3:
        completionTimes = events.completionTimes_customer_support;
        break;
    case 4:
        completionTimes = events.completionTimes_security_check;
        break;
    case 5:
        completionTimes = events.completionTimes_ticket_gate;
        break;
    default:
        printf("Error: the value of param index must be a number beteween 1 and 5!\n");
        fflush(stdout);
        exit(-8);
    }

    for (int i = 0; i < num_servers; i++)
    {
        if (completionTimes[i] < min->completionTime)
        {
            min->serverOffset = i;
            min->completionTime = completionTimes[i];
        }
    }

    return min;
}

double get_smallest(double *values, int len)
{

    double smallest = (double)INFINITY;

    for (int i = 0; i < len; i++)
    {
        if (values[i] < smallest)
            smallest = values[i];
    }

    return smallest;
}

double get_minimum_time(struct event_list events, struct states *state, int *n)
{
    double min_abandon_ticket_machine = (double)INFINITY;
    double min_abandon_ticket_office = (double)INFINITY;
    double min_abandon_customer_support = (double)INFINITY;
    double min_abandon_security_check = (double)INFINITY;
    double min_abandon_ticket_gate = (double)INFINITY;

    struct next_abandon *abandon_ticket_machine = NULL;
    struct next_abandon *abandon_ticket_office = NULL;
    struct next_abandon *abandon_customer_support = NULL;
    struct next_abandon *abandon_security_check = NULL;
    struct next_abandon *abandon_ticket_gate = NULL;

    if (events.head_ticket_machine != NULL)
    {

        abandon_ticket_machine = get_min_abandon(events.head_ticket_machine);
        min_abandon_ticket_machine = abandon_ticket_machine->abandonTime;
    }

    if (events.head_ticket_office != NULL)
    {
        abandon_ticket_office = get_min_abandon(events.head_ticket_office);
        min_abandon_ticket_office = abandon_ticket_office->abandonTime;
    }

    if (events.head_customer_support != NULL)
    {
        abandon_customer_support = get_min_abandon(events.head_customer_support);
        min_abandon_ticket_office = abandon_customer_support->abandonTime;
    }

    if (events.head_security_check != NULL)
    {
        abandon_security_check = get_min_abandon(events.head_security_check);
        min_abandon_ticket_office = abandon_security_check->abandonTime;
    }

    if (events.head_ticket_gate != NULL)
    {
        abandon_ticket_gate = get_min_abandon(events.head_ticket_gate);
        min_abandon_ticket_office = abandon_ticket_gate->abandonTime;
    }

    struct next_job *next_queue_time_ticket_machine = get_min_queue_time(events, n[0], state[0].server_occupation, 1);
    struct next_job *next_queue_time_ticket_office = get_min_queue_time(events, n[1], state[1].server_occupation, 2);
    struct next_job *next_queue_time_customer_support = get_min_queue_time(events, n[2], state[2].server_occupation, 3);
    struct next_job *next_queue_time_security_check = get_min_queue_time(events, n[3], state[3].server_occupation, 4);
    struct next_job *next_queue_time_ticket_gate = get_min_queue_time(events, n[4], state[4].server_occupation, 5);

    double min_service_ticket_machine = next_queue_time_ticket_machine->completionTime;
    double min_service_ticket_office = next_queue_time_ticket_office->completionTime;
    double min_service_customer_support = next_queue_time_customer_support->completionTime;
    double min_service_security_check = next_queue_time_security_check->completionTime;
    double min_service_ticket_gate = next_queue_time_ticket_gate->completionTime;

    double times[DIM];

    times[0] = min_abandon_ticket_machine;
    times[1] = min_abandon_ticket_office;
    times[2] = min_abandon_customer_support;
    times[3] = min_abandon_security_check;
    times[4] = min_abandon_ticket_gate;
    times[5] = min_service_ticket_machine;
    times[6] = min_service_ticket_office;
    times[7] = min_service_customer_support;
    times[8] = min_service_security_check;
    times[9] = min_service_ticket_gate;
    times[10] = events.user_arrival_to_ticket_machine.user_arrival_time;
    times[11] = events.user_arrival_to_ticket_office.user_arrival_time;
    times[12] = events.user_arrival_to_customer_support.user_arrival_time;
    times[13] = events.user_arrival_to_security_check.user_arrival_time;
    times[14] = events.user_arrival_to_ticket_gate.user_arrival_time;

    free(abandon_ticket_machine);
    free(abandon_ticket_office);
    free(abandon_customer_support);
    free(abandon_security_check);
    free(abandon_ticket_gate);

    free(next_queue_time_ticket_machine);
    free(next_queue_time_ticket_office);
    free(next_queue_time_customer_support);
    free(next_queue_time_security_check);
    free(next_queue_time_ticket_gate);

    return get_smallest(times, DIM);
}

int get_total_busy_servers(int num_servers, int *server_list)
{
    int count = 0;
    for (int i = 0; i < num_servers; i++)
    {
        if (server_list[i] != 0 && server_list[i] != -3)
            count++;
    }

    return count;
}