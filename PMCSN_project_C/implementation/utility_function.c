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
#include "./../headers/simulation.h"
#include "./../data_structures/area.h"
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

    bool flag = true;

    // This if check if there are job in the various centers
    if (state[0].population > 0 || state[1].population > 0 || state[2].population > 0 || state[3].population > 0 || state[4].population > 0)
        flag = false;

    // This for checks if there are jobs in the various server, both in the centers and in the queues
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < n[j]; i++)
        {
            if (state[j].server_occupation[i] != 0)
            {
                flag = false;
                break;
            }
        }
    }

    return flag;
}

struct next_abandon *get_min_abandon(struct abandon_node *head)
{
    // GetMinAbandon
    struct next_abandon *min = (struct next_abandon *)malloc(sizeof(struct next_abandon));

    if (head != NULL)
    {
        min->user_Id = head->id;
        min->abandonTime = head->abandon_time;
    }

    struct abandon_node *current = head;

    while (current != NULL)
    {
        if (current->abandon_time < min->abandonTime)
        {
            min->user_Id = current->id;
            min->abandonTime = current->abandon_time;
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
        if (index == 5 || index == 3)
            printf("completionTimes[%d] = %f\n", i, completionTimes[i]);
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
    double times[DIM];

    for (int i = 0; i < DIM; i++)
    {
        times[i] = 0.0;
    }

    double min_abandon_ticket_machine = (double)INFINITY;
    double min_abandon_ticket_office = (double)INFINITY;
    double min_abandon_customer_support = (double)INFINITY;

    struct next_abandon *abandon_ticket_machine = NULL;
    struct next_abandon *abandon_ticket_office = NULL;
    struct next_abandon *abandon_customer_support = NULL;

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
        min_abandon_customer_support = abandon_customer_support->abandonTime;
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

    times[0] = min_abandon_ticket_machine;
    times[1] = min_abandon_ticket_office;
    times[2] = min_abandon_customer_support;
    times[3] = min_service_ticket_machine;
    times[4] = min_service_ticket_office;
    times[5] = min_service_customer_support;
    times[6] = min_service_security_check;
    times[7] = min_service_ticket_gate;
    times[8] = events.user_who_has_purchased_ticket.user_arrival_time;
    times[9] = events.user_arrival_to_ticket_machine.user_arrival_time;
    times[10] = events.user_arrival_to_ticket_office.user_arrival_time;
    times[11] = events.user_arrival_to_customer_support.user_arrival_time;
    times[12] = events.user_arrival_to_ticket_gate.user_arrival_time;

    for (int i = 0; i < DIM; i++)
    {
        printf("times[%d] = %f\n", i, times[i]);
    }

    free(abandon_ticket_machine);
    free(abandon_ticket_office);
    free(abandon_customer_support);

    free(next_queue_time_ticket_machine);
    free(next_queue_time_ticket_office);
    free(next_queue_time_customer_support);
    free(next_queue_time_security_check);
    free(next_queue_time_ticket_gate);

    return get_smallest(times, DIM);
}

int get_total_busy_servers(int num_servers, int *server_occupation)
{
    int count = 0;
    for (int i = 0; i < num_servers; i++)
    {
        if (server_occupation[i] != 0 /*&& server_list[i] != -3*/)
            count++;
    }

    return count;
}

void feedback(struct event_list *events, struct time *time, double rate)
{
    struct states *state = get_first_state_address();
    struct loss *loss = get_first_loss();
    user_arrivals_ticket_office(events, time, &state[1], &loss[1], rate);
}

void routing_ticket_purchased(struct event_list *events, struct time *time, double rate)
{
    struct states *state = get_first_state_address();
    struct loss *loss = get_first_loss();

    if (Random() <= P_OF_CUSTOMER_SUPPORT)
    {
        printf("Route to customer support!\n");
        struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
        if (!job)
        {
            printf("Error in malloc in routing_ticket_purchased to customer support!\n");
            exit(-1);
        }

        job = events->head_ticket_purchased;
        events->head_ticket_purchased = job->next;
        job->prev = NULL;

        if (events->head_queue_customer_support == NULL)
        {
            events->head_queue_customer_support = job;
            events->tail_queue_customer_support = job;
            job->next = NULL;
            job->prev = NULL;
        }
        else
        {
            events->tail_queue_customer_support->next = job;
            job->prev = events->tail_queue_customer_support;
            job->next = NULL;
            events->tail_queue_customer_support = job;
        }

        job = NULL;
        user_arrivals_customer_support(events, time, &state[2], &loss[2], rate);
    }
    else
    {
        // da ticket purchased a head user_security_check
        struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
        if (!job)
        {
            printf("Error in malloc in routing ticket purchased to ticket gate!\n");
            exit(-1);
        }
        job = events->head_ticket_purchased;
        events->head_ticket_purchased = events->head_ticket_purchased->next;

        if (events->head_user_to_security_check == NULL)
        {
            events->head_user_to_security_check = job;
            events->tail_user_to_security_check = job;
            job->prev = NULL;
            job->next = NULL;
        }
        else
        {
            events->tail_user_to_security_check->next = job;
            job->prev = events->tail_user_to_security_check;
            job->next = NULL;
            events->tail_user_to_security_check = job;
        }
        job = NULL;
        routing_security_check(events, time, rate);
    }
}

void routing_security_check(struct event_list *events, struct time *time, double rate)
{
    struct states *state = get_first_state_address();
    struct loss *loss = get_first_loss();

    if (Random() <= P_OF_SECURITY_CHECK)
    {
        printf("Route to security check queue!\n");
        struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
        if (!job)
        {
            printf("Error in malloc in routing security check to ticket gate!\n");
            exit(-1);
        }

        job = events->head_user_to_security_check;
        events->head_user_to_security_check = events->head_user_to_security_check->next;

        if (events->head_security_check_queue == NULL)
        {
            events->head_security_check_queue = job;
            events->tail_security_check_queue = job;
            job->prev = NULL;
            job->next = NULL;
        }
        else
        {
            events->tail_security_check_queue->next = job;
            job->prev = events->tail_security_check_queue;
            job->next = NULL;
            events->tail_security_check_queue = job;
        }
        job = NULL;
        user_arrivals_security_check(events, time, &state[3], &loss[3], rate);
    }
    else
    {
        struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
        if (!job)
        {
            printf("Error in malloc in routing security check to ticket gate!\n");
            exit(-1);
        }
        job = events->head_user_to_security_check;
        events->head_user_to_security_check = events->head_user_to_security_check->next;

        if (events->head_ticket_gate == NULL)
        {
            events->head_ticket_gate = job;
            events->tail_ticket_gate = job;
            job->prev = NULL;
            job->next = NULL;
        }
        else
        {
            events->tail_ticket_gate->next = job;
            job->prev = events->tail_ticket_gate;
            job->next = NULL;
            events->tail_ticket_gate = job;
        }
        job = NULL;
        user_arrivals_ticket_gate(events, time, &state[4], &loss[4]);
    }
}
void routing_ticket_gate(struct event_list *events, struct time *time)
{
    struct states *state = get_first_state_address();
    struct loss *loss = get_first_loss();
    user_arrivals_ticket_gate(events, time, &state[4], &loss[4]);
}

void verify(struct area *area, struct loss *loss, double time)
{

    printf("time: %f\n", time);

    double rho_0 = area[0].service / time;
    double q_0 = area[0].queue / time;
    double n_0 = area[0].node / time;

    printf("rho_0 = %f\n", rho_0);
    printf("q_0 = %f\n", q_0);
    printf("n_0 = %f\n", n_0);

    if (n_0 - (q_0 + rho_0) > 0.00001)
        exit(-100);

    double rho_1 = area[1].service / time;
    double q_1 = area[1].queue / time;
    double n_1 = area[1].node / time;

    printf("rho_1 = %f\n", rho_1);
    printf("q_1 = %f\n", q_1);
    printf("n_1 = %f\n", n_1);

    if (n_1 - (q_1 + rho_1) > 0.00001)
        exit(-101);

    double rho_2 = area[2].service / time;
    double q_2 = area[2].queue / time;
    double n_2 = area[2].node / time;

    printf("rho_2 = %f\n", rho_2);
    printf("q_2 = %f\n", q_2);
    printf("n_2 = %f\n", n_2);

    if (n_2 - (q_2 + rho_2) > 0.00001)
        exit(-102);

    double rho_3 = area[3].service / time;
    double n_3 = area[3].node / time;

    printf("rho_3 = %f\n", rho_3);
    printf("n_3 = %f\n", n_3);

    if (n_3 - rho_3 > 0.00001)
        exit(-103);

    double rho_4 = area[4].service / time;
    double q_4 = area[4].queue / time;
    double n_4 = area[4].node / time;

    printf("rho_4 = %f\n", rho_4);
    printf("q_4 = %f\n", q_4);
    printf("n_4 = %f\n", n_4);

    if (n_4 - (q_4 + rho_4) > 0.00001)
        exit(-104);

    double rho_00 = area[0].service / (loss[0].index_user);
    double q_00 = area[0].queue / (loss[0].index_user);
    double n_00 = area[0].node / (loss[0].index_user);
    if (n_00 - (q_00 + rho_00) > 0.00001)
        exit(-110);

    double rho_01 = area[1].service / (loss[1].index_user);
    double q_01 = area[1].queue / (loss[1].index_user);
    double n_01 = area[1].node / (loss[1].index_user);
    printf("rho_01 %f, q_01 %f, n_01, %f\n", rho_01, q_01, n_01);
    if (n_01 - (q_01 + rho_01) > 0.00001)
        exit(-111);

    double rho_02 = area[2].service / (loss[2].index_user);
    double q_02 = area[2].queue / (loss[2].index_user);
    double n_02 = area[2].node / (loss[2].index_user);
    printf("rho_02 %f, q_02 %f, n_02, %f\n", rho_02, q_02, n_02);
    if (n_02 - (q_02 + rho_02) > 0.00001)
        exit(-112);

    double rho_03 = area[3].service / (loss[3].index_user);
    double n_03 = area[3].node / (loss[3].index_user);
    printf("rho_03 %f, n_03, %f\n", rho_03, n_03);
    if (n_03 - rho_03 > 0.00001)
        exit(-113);

    double rho_04 = area[4].service / (loss[4].index_user);
    double q_04 = area[4].queue / (loss[4].index_user);
    double n_04 = area[4].node / (loss[4].index_user);
    printf("rho_04 %f, q_04 %f, n_04, %f\n", rho_04, q_04, n_04);
    if (n_04 - (q_04 + rho_04) > 0.00000000000000001)
        exit(-114);
}
