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

		state->server_count += 1;
		state->server_occupation[idle_offset] = 1;
		events->completionTimes_security_check[idle_offset] = get_security_check_departure(time->current);

		processed_job_security_check[idle_offset] = events->head_security_check_queue->id;

		dequeue_node_free_node(&events->head_security_check_queue);
	}
	else if (idle_offset == -1)
	{
		state->queue_count += 1;
	}

	state->population = state->queue_count + state->server_count;
}

void user_departure_security_check(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	state->server_count -= 1;

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
		dequeue_node_free_node(&events->head_security_check_queue);
		state->queue_count -= 1;
		state->server_count += 1;
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

		job->id = processed_job_security_check[server_offset];
		job->arrival_time = time->current;

		enqueue_node(&events->head_ticket_gate, &events->tail_ticket_gate, job);
		routing_ticket_gate(events, time);
	}
	state->population = state->queue_count + state->server_count;
}