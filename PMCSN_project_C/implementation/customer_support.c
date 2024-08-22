#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/customer_support.h"
#include "../headers/ticket_office.h"
#include "../data_structures/event_list.h"

int processed_job_customer_support[NUMBER_OF_CUSTOMER_SUPPORT_SERVER];

double get_customer_support_departure(double start)
{
	SelectStream(7);
	double departure = start + Exponential(SR_CUSTOMER_SUPPORT_OPERATOR);
	return departure;
}

double get_abandon_customer_support(double start)
{
	SelectStream(17);
	double departure = start + Exponential(360);
	return departure;
}

void user_arrivals_customer_support(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{
	loss->index_user += 1;

	// events->user_arrival_to_customer_support.user_arrival_time = events->head_queue_customer_support->arrival_time;

	int idle_offset = -1;
	for (int i = 0; i < NUMBER_OF_CUSTOMER_SUPPORT_SERVER; i++)
	{
		if (state->server_occupation[i] == 0)
		{
			idle_offset = i;
			break;
		}
	}

	if (get_random(8) <= P_LEAVE_CUSTOMER_SUPPORT)
	{
		// abandon is here
		printf("Random choose abandon customer support!\n");
		struct queue_node *abandon_job;
		abandon_job = dequeue_node(&events->head_queue_customer_support);
		abandon_job->id = loss->index_user;
		abandon_job->arrival_time = get_abandon_customer_support(time->current);
		enqueue_node(&events->head_customer_support, &events->tail_customer_support, abandon_job);
	}
	else
	{
		if (idle_offset >= 0 && events->head_queue_customer_support != NULL)
		{
			/*
			Caso in cui la coda non è vuota e si è liberato un server, quindi
			mandiamo il primo della coda in servizio e spostiamo il puntatore
			della testa al prossimo nodo e liberiamo memoria.
			*/

			state->server_count++;
			state->server_occupation[idle_offset] = 1;
			events->completionTimes_customer_support[idle_offset] = get_customer_support_departure(time->current);

			dequeue_node_free_node(&events->head_queue_customer_support);

			/*
			Aggiorniamo i dati relativi al nodo che è stato appena messo in
			coda
			*/

			events->tail_queue_customer_support->id = loss->index_user;
			time->last[2] = time->current;
		}
		else if (idle_offset == -1)
		{
			state->queue_count++;
		}
		state->population = state->queue_count + state->server_count;
	}
}

void user_departure_customer_support(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset, double rate)
{
	state->server_count--;
	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in user departure in customer support!\n");
		exit(-1);
	}

	/*
	Mi prendo i dati del job che ha appena terminato il servizio.
	*/

	time->last[2] = time->current;
	tail_job->id = processed_job_customer_support[server_offset];
	tail_job->arrival_time = events->completionTimes_customer_support[server_offset];

	if (state->queue_count > 0)
	{
		/*
		Il job in servizio viene messo o in coda al prossimo centro oppure viene mandato
		in ticket office per via del feedback. Se c'è un job in coda, assegno questo job
		al servente che si è appena liberato e libero memoria dalla coda dei job di
		customer support.
		*/

		events->completionTimes_customer_support[server_offset] = get_customer_support_departure(time->current);
		state->server_occupation[server_offset] = 1;
		dequeue_node_free_node(&events->head_queue_customer_support);
		state->queue_count--;
		state->server_count++;
	}
	else
	{
		/*
		Il servente ha terminato la sua esecuzione e resettiamo il suo stato perché non
		c'è più nessun altro job da processare.
		*/

		events->completionTimes_customer_support[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	// feedback is here
	if (get_random(9) <= P_OF_CHANGE_TICKET)
	{
		printf("Feedback event\n");
		/*
		Se il job fa il feedback, libero memoria perché i primi due centri non tangono
		conto dei dati dei job che arrivato.
		*/
		free(tail_job);
		feedback(events, time, rate);
	}
	else
	{
		enqueue_node(&events->head_user_to_security_check, &events->tail_user_to_security_check, tail_job);
		routing_security_check(events, time, rate);
	}
	state->population = state->queue_count + state->server_count;
}

void abandon_customer_support(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	printf("Abandon customer support!\n");

	struct queue_node *current = events->head_customer_support;
	while (current != NULL)
	{
		if (current->id == job_id)
			break;
		current = current->next;
	}

	if (current == NULL)
	{
		printf("Job ID %d not found in the queue!\n", job_id);
		return;
	}

	struct queue_node *prev = current->prev;
	struct queue_node *next = current->next;

	if (prev != NULL)
	{
		prev->next = next;
	}
	else
	{
		events->head_customer_support = next;
	}

	if (next != NULL)
	{
		next->prev = prev;
	}
	else
	{
		events->tail_customer_support = prev;
	}

	free(current);
	loss->loss_user += 1;
}
