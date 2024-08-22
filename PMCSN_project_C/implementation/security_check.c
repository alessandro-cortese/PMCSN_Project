#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/security_check.h"
#include "../data_structures/event_list.h"

int processed_job_security_check[NUMBER_OF_SECURITY_CHECK_SERVERS];

double get_security_check_departure(double start)
{
	SelectStream(10);
	double departure = start + Exponential(SR_SECURITY_CONTROL_OPERATOR);
	return departure;
}

void user_arrivals_security_check(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{
	printf("Arrivals security check\n");
	printf("state->queue_count = %d\n", state->queue_count);
	printf("state->server_count = %d\n", state->server_count);
	struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!job)
	{
		printf("Error in malloc in user arrival in security check!\n");
		exit(-1);
	}

	loss->index_user += 1;

	time->last[3] = time->current;
	events->tail_security_check_queue->id = loss->index_user;

	int idle_offset = -1;
	for (int i = 0; i < NUMBER_OF_SECURITY_CHECK_SERVERS; i++)
	{
		printf("state->server_occupation[%d] = %d\n", i, state->server_occupation[i]);
		if (state->server_occupation[i] == 0)
		{
			idle_offset = i;
			break;
		}
	}

	if (idle_offset >= 0 && events->head_security_check_queue != NULL)
	{

		/*
		Caso in cui la coda non è vuota e si è liberato un server, quindi
		mandiamo il primo della coda in servizio e spostiamo il puntatore
		della testa al prossimo nodo e liberiamo memoria.
		*/

		state->server_count++;
		state->server_occupation[idle_offset] = 1;
		events->completionTimes_security_check[idle_offset] = get_security_check_departure(time->current);

		processed_job_security_check[idle_offset] = events->head_security_check_queue->id;

		dequeue_node_free_node(&events->head_security_check_queue);
	}
	else if (idle_offset == -1)
	{
		state->queue_count++;
	}

	state->population = state->queue_count + state->server_count;
	printf("idle_offset = %d\n", idle_offset);
	printf("Arrivals security check, fine evento\n");
	printf("state->queue_count = %d\n", state->queue_count);
	printf("state->server_count = %d\n", state->server_count);
}

void user_departure_security_check(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	printf("Departure security check\n");
	printf("state->queue_count = %d\n", state->queue_count);
	printf("state->server_count = %d\n", state->server_count);
	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	state->server_count--;

	// Inserimento in coda di un nuovo nodo all'interno della lista degli arrivi al ticket gate
	struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!job)
	{
		printf("Error in malloc in user departure in security check!\n");
		exit(-1);
	}

	if (state->queue_count > 0)
	{

		/*
		Il job in servizio viene messo o in coda al prossimo centro oppure viene mandato
		in ticket gate. Se c'è un job in coda, assegno questo job al servente che si è
		appena liberato e libero memoria dalla coda dei job di security check.
		*/

		processed_job_security_check[server_offset] = events->head_security_check_queue->id;
		state->server_occupation[server_offset] = 1;
		events->completionTimes_security_check[server_offset] = get_security_check_departure(time->current);
		// events->head_security_check_queue = events->head_security_check_queue->next;
		dequeue_node_free_node(&events->head_security_check_queue);
		state->queue_count--;
		state->server_count++;
		printf("Dentro la departure\n");
		printf("state->queue_count = %d\n", state->queue_count);
		printf("state->server_count = %d\n", state->server_count);
		printf("lenOfQueue(events->head_security_check_queue) = %d\n", lenOfQueue(events->head_security_check_queue));
	}
	else
	{

		/*
		Il servente ha terminato la sua esecuzione e resettiamo il suo stato perché non
		c'è più nessun altro job da processare.
		*/

		events->completionTimes_security_check[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	if (!(get_random(11) <= P_LEAVE_SECURITY_CONTROL))
	{

		printf("prima\n");
		job->id = processed_job_security_check[server_offset];
		job->arrival_time = time->current;

		// if (events->head_ticket_gate == NULL)
		// {
		// 	tail->id = processed_job_security_check[server_offset];
		// 	events->head_ticket_gate = tail;
		// 	events->head_ticket_gate->prev = NULL;
		// 	events->head_ticket_gate->next = NULL;
		// 	events->tail_ticket_gate = tail;
		// }
		// else if (events->head_ticket_gate != NULL)
		// {
		// 	tail->id = processed_job_security_check[server_offset];
		// 	events->tail_ticket_gate->next = tail;
		// 	tail->prev = events->tail_ticket_gate;
		// 	tail->next = NULL;
		// 	events->tail_ticket_gate = tail;
		// }

		// tail = NULL;

		enqueue_node(&events->head_ticket_gate, &events->tail_ticket_gate, job);
		routing_ticket_gate(events, time);
	}
	else
	{
		printf("Job have false document\n");
		// prob of having false document or other problems
		events->completionTimes_security_check[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	state->population = state->queue_count + state->server_count;
	printf("Departure security check, fine funzione\n");
	printf("state->queue_count = %d\n", state->queue_count);
	printf("state->server_count = %d\n", state->server_count);
}