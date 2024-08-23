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

double get_random(int index)
{
    double random;
    SelectStream(index);
    random = Random();
    return random;
}

double Exponential(double m)
{
    return (-m * log(1.0 - Random()));
}
int lenOfQueue(struct queue_node *head)
{
    int length = 0;
    struct queue_node *current = head;

    // Traversing the linked list until the end
    while (current != NULL)
    {
        length++;                // Increment length for each node
        current = current->next; // Move to the next node
    }

    return length;
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

struct next_abandon *get_min_abandon(struct queue_node *head)
{
    // GetMinAbandon
    struct next_abandon *min = (struct next_abandon *)malloc(sizeof(struct next_abandon));

    if (head != NULL)
    {
        min->user_Id = head->id;
        min->abandonTime = head->arrival_time;
    }

    struct queue_node *current = head;

    while (current != NULL)
    {
        if (current->arrival_time < min->abandonTime)
        {
            min->user_Id = current->id;
            min->abandonTime = current->arrival_time;
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
    user_arrivals_ticket_office_feedback(events, time, &state[1], &loss[1], rate);
}

void routing_ticket_purchased(struct event_list *events, struct time *time, double rate)
{
    struct states *state = get_first_state_address();
    struct loss *loss = get_first_loss();

    if (get_random(13) <= P_OF_CUSTOMER_SUPPORT)
    {
        // printf("Route to customer support!\n");
        struct queue_node *job;
        job = dequeue_node(&events->head_ticket_purchased);
        enqueue_node(&events->head_queue_customer_support, &events->tail_queue_customer_support, job);
        user_arrivals_customer_support(events, time, &state[2], &loss[2], rate);
    }
    else
    {
        // printf("Routing to security check\n");
        //  da ticket purchased a head user_security_check
        struct queue_node *job;

        job = dequeue_node(&events->head_ticket_purchased);
        enqueue_node(&events->head_user_to_security_check, &events->tail_user_to_security_check, job);
        routing_security_check(events, time, rate);
    }
}

void routing_security_check(struct event_list *events, struct time *time, double rate)
{
    struct states *state = get_first_state_address();
    struct loss *loss = get_first_loss();

    if (get_random(14) <= P_OF_SECURITY_CHECK)
    {
        // printf("Route to security check queue!\n");
        struct queue_node *job;
        job = dequeue_node(&events->head_user_to_security_check);
        enqueue_node(&events->head_security_check_queue, &events->tail_security_check_queue, job);
        user_arrivals_security_check(events, time, &state[3], &loss[3], rate);
    }
    else
    {
        // printf("Route to ticket gate queue!\n");
        struct queue_node *job;
        job = dequeue_node(&events->head_user_to_security_check);
        enqueue_node(&events->head_ticket_gate, &events->tail_ticket_gate, job);
        user_arrivals_ticket_gate(events, time, &state[4], &loss[4]);
    }
}
void routing_ticket_gate(struct event_list *events, struct time *time)
{
    struct states *state = get_first_state_address();
    struct loss *loss = get_first_loss();
    user_arrivals_ticket_gate(events, time, &state[4], &loss[4]);
}
void consistency_check_population(struct event_list *events)
{
    struct states *state = get_first_state_address();
    if (state[0].population < 0)
    {
        printf("Population for ticket machine is negative, exiting...\n");
        exit(-1);
    }
    if (state[1].population < 0)
    {
        printf("Population for ticket office is negative, exiting...\n");
        exit(-1);
    }
    if (state[2].population < 0)
    {
        printf("Population for customer support is negative, exiting...\n");
        exit(-1);
    }
    if (state[3].population < 0)
    {
        printf("Population for security check is negative, exiting...\n");
        exit(-1);
    }
    if (state[4].population < 0)
    {
        printf("Population for ticket gate is negative, exiting...\n");
        exit(-1);
    }
    if (state[0].server_count > NUMBER_OF_TICKET_MACHINE_SERVER)
    {
        printf("Errore di consistenza nei server di ticket machine, assegnati più di quelli disponibili!\n");
        exit(-1);
    }
    if (state[1].server_count > NUMBER_OF_TICKET_OFFICE_SERVER)
    {
        printf("Errore di consistenza nei server di ticket office, assegnati più di quelli disponibili!\n");
        exit(-1);
    }

    if (state[2].queue_count != lenOfQueue(events->head_queue_customer_support))
    {
        printf("queue count for customer support is different!\n");
        printf("len queue customer support is %d\n", lenOfQueue(events->head_queue_customer_support));
        printf("state[2].queue_count = %d\n", state[2].queue_count);
        printf("state[2].server_count = %d\n", state[2].server_count);
        exit(-1);
    }
    if (state[2].server_count > NUMBER_OF_CUSTOMER_SUPPORT_SERVER)
    {
        printf("Errore di consistenza nei server di customer support, assegnati più di quelli disponibili!\n");
        exit(-1);
    }
    if (state[3].queue_count != lenOfQueue(events->head_security_check_queue))
    {
        printf("queue count for security check is different!\n");
        printf("len queue security check is %d\n", lenOfQueue(events->head_security_check_queue));
        printf("state[3].queue_count = %d\n", state[3].queue_count);
        exit(-1);
    }
    if (state[3].server_count > NUMBER_OF_SECURITY_CHECK_SERVERS)
    {
        printf("Errore di consistenza nei server di security check, assegnati più di quelli disponibili!\n");
        exit(-1);
    }
    if (state[4].queue_count != lenOfQueue(events->head_ticket_gate))
    {
        printf("len queue ticket gate is %d\n", lenOfQueue(events->head_ticket_gate));
        printf("state[4].queue_count = %d\n", state[4].queue_count);
        exit(-1);
    }
    if (state[4].server_count > NUMBER_OF_TICKET_GATE_SERVERS)
    {
        printf("Errore di consistenza nei server di ticket gate, assegnati più di quelli disponibili!\n");
        exit(-1);
    }
    if (state[4].queue_count == 1 && state[4].server_count < 15)
    {
        struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
        if (!job)
        {
            printf("Error in malloc in routing security check to ticket gate!\n");
            exit(-1);
        }
        printf("Consistency Error in queue of ticket gate!\n");
        exit(-1);
    }
}

void enqueue_node(struct queue_node **head, struct queue_node **tail, struct queue_node *job)
{
    if (*head == NULL)
    {
        (*head) = job;
        (*tail) = job;
        job->next = NULL;
        job->prev = NULL;
    }
    else if (*head != NULL)
    {
        (*tail)->next = job;
        job->prev = (*tail);
        job->next = NULL;
        (*tail) = job;
    }
    job = NULL;
}

void dequeue_node_free_node(struct queue_node **head)
{
    struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
    if (!job)
    {
        printf("Error in malloc in dequeue_node_free_node!\n");
        exit(-1);
    }

    job = (*head);
    (*head) = (*head)->next;
    free(job);
}

struct queue_node *dequeue_node(struct queue_node **head)
{
    struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
    if (!job)
    {
        printf("Error in malloc in dequeue_node!\n");
        exit(-1);
    }

    job = (*head);
    (*head) = (*head)->next;

    return job;
}

void verify(struct area *a, struct loss *loss, double t, struct time *time)
{

    // printf("time: %f\n", t);

    double rho_1 = a[0].service / (t * NUMBER_OF_TICKET_MACHINE_SERVER);
    double rho_2 = a[1].service / (t * NUMBER_OF_TICKET_OFFICE_SERVER);
    double rho_3 = a[2].service / (t * NUMBER_OF_CUSTOMER_SUPPORT_SERVER);
    double rho_4 = a[3].service / (t * NUMBER_OF_SECURITY_CHECK_SERVERS);
    double rho_5 = a[4].service / (t * NUMBER_OF_TICKET_GATE_SERVERS);

    // popolazione media nelle code
    double q_1 = a[0].queue / t;
    double q_2 = a[1].queue / t;
    double q_3 = a[2].queue / t;
    double q_4 = a[3].queue / t;
    double q_5 = a[4].queue / t;

    // popolazione media nel centro
    double n_1 = a[0].node / t;
    double n_2 = a[1].node / t;
    double n_3 = a[2].node / t;
    double n_4 = a[3].node / t;
    double n_5 = a[4].node / t;

    // tempo di servizio medio
    double serv0 = a[0].service / (loss[0].index_user);
    double serv1 = a[1].service / (loss[1].index_user);
    double serv2 = a[2].service / (loss[2].index_user);
    double serv3 = a[3].service / (loss[3].index_user);
    double serv4 = a[4].service / (loss[4].index_user);

    // tempo di attesa medio nella coda
    double delay1 = a[0].queue / (loss[0].index_user);
    double delay2 = a[1].queue / (loss[1].index_user);
    double delay3 = a[2].queue / (loss[2].index_user);
    double delay4 = a[3].queue / (loss[3].index_user);
    double delay5 = a[4].queue / (loss[4].index_user);

    // tempo di risposta medio
    double wait1 = a[0].node / (loss[0].index_user);
    double wait2 = a[1].node / (loss[1].index_user);
    double wait3 = a[2].node / (loss[2].index_user);
    double wait4 = a[3].node / (loss[3].index_user);
    double wait5 = a[4].node / (loss[4].index_user);

    // tempo di interarrivo
    double interArr1 = time->last[0] / (loss[0].index_user);
    double interArr2 = time->last[1] / (loss[1].index_user);
    double interArr3 = time->last[2] / (loss[2].index_user);
    double interArr4 = time->last[3] / (loss[3].index_user);
    double interArr5 = time->last[4] / (loss[4].index_user);

    // numero arrivi famiglie
    double fam1 = loss[0].index_user;
    double fam2 = loss[1].index_user;
    double fam3 = loss[2].index_user;
    double fam4 = loss[3].index_user;
    double fam5 = loss[4].index_user;

    // printf("Ticket Machine\n");
    // printf("rho_1 = %f\n", rho_1);
    // printf("E(N_q1) = %f\n", q_1);
    // printf("E(n_1) = %f\n", n_1);
    // printf("E(S_1) = %f\n", serv0);
    // printf("E(Tq_1) = %.6f\n", delay1);
    // printf("E(T_s1) = %f\n", wait1);
    // printf("interArr1 = %f\n", interArr1);
    // printf("loss 1 = %f\n\n", fam1);

    // printf("Ticket Office\n");
    // printf("rho_2 = %f\n", rho_2);
    // printf("E(N_q2) = %f\n", q_2);
    // printf("E(n_2) = %f\n", n_2);
    // printf("E(S_2) = %f\n", serv1);
    // printf("E(Tq_2) = %.6f\n", delay2);
    // printf("E(T_s2) = %f\n", wait2);
    // printf("interArr2 = %f\n", interArr2);
    // printf("loss 2 = %f\n\n", fam2);

    // printf("Customer Support\n");
    // printf("rho_3 = %f\n", rho_3);
    // printf("E(N_q3) = %f\n", q_3);
    // printf("E(n_3) = %f\n", n_3);
    // printf("E(S_3) = %f\n", serv2);
    // printf("E(Tq_3) = %.6f\n", delay3);
    // printf("E(T_s3) = %f\n", wait3);
    // printf("interArr3 = %f\n", interArr3);
    // printf("loss 3 = %f\n\n", fam3);

    // printf("Security Check\n");
    // printf("rho_4 = %f\n", rho_4);
    // printf("E(N_q4) = %f\n", q_4);
    // printf("E(n_4) = %f\n", n_4);
    // printf("E(S_4) = %f\n", serv3);
    // printf("E(Tq_4) = %.6f\n", delay4);
    // printf("E(T_s4) = %f\n", wait4);
    // printf("interArr4 = %f\n", interArr4);
    // printf("loss 4 = %f\n\n", fam4);

    // printf("Ticket Gate\n");
    // printf("rho_5 = %f\n", rho_5);
    // printf("E(N_q5) = %f\n", q_5);
    // printf("E(n_5) = %f\n", n_5);
    // printf("E(S_5) = %f\n", serv4);
    // printf("E(Tq_5) = %.6f\n", delay5);
    // printf("E(T_s5) = %f\n", wait5);
    // printf("interArr5 = %f\n", interArr5);
    // printf("loss 5 = %f\n\n", fam5);
}

void print_progress_bar(double part, double total, double oldPart)
{
    double percentage = (part / total) * 100;
    double oldPercentage = (oldPart / total) * 100;

    if ((int)oldPercentage == (int)percentage && part != total)
    {
        return; // Evita di aggiornare se la percentuale non è cambiata
    }

    // Se è l'ultima iterazione, forza la barra al 100%
    if (part == total)
    {
        percentage = 100;
    }

    printf("\rSimulation Progress: |");

    // Stampa blocchi pieni in base alla percentuale completata
    for (int i = 0; i <= percentage / 2; i++)
    {
        printf("█");
        fflush(stdout);
    }

    // Stampa spazi vuoti per il restante della barra
    for (int j = percentage / 2; j < 50; j++)
    {
        printf(" ");
    }

    printf("| %02.0f%%", percentage); // Stampa la percentuale alla fine della barra

    // Forza la visualizzazione immediata della barra di avanzamento
    fflush(stdout);
}
