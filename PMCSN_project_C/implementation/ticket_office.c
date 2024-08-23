#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/ticket_office.h"
#include "../data_structures/event_list.h"
#include "../headers/utility_functions.h"

double get_user_arrival_to_ticket_office(double arrival, double rate)
{
	SelectStream(0);
	arrival += Exponential(rate / P_TICKET_PURCHASED_FROM_TICKET_OFFICE);
	return (arrival);
}

double get_ticket_office_departure(double start)
{
	SelectStream(1);
	double departure = start + Exponential(SR_TICKET_OFFICE_OPERATOR);
	return departure;
}

double get_abandon_ticket_office(double start)
{
	SelectStream(15);
	double departure = start + Exponential(360);
	return departure;
}

void user_arrivals_ticket_office(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate, double stop)
{
	events->user_arrival_to_ticket_office.user_arrival_time = get_user_arrival_to_ticket_office(time->current, rate);

	time->last[1] = time->current;

	if (events->user_arrival_to_ticket_office.user_arrival_time > stop)
	{
		events->user_arrival_to_ticket_office.user_arrival_time = (double)INFINITY;
		events->user_arrival_to_ticket_office.is_user_arrival_active = false;
		// printf("Stop arrival to ticket office!\n");
	}
	else
	{
		loss->index_user += 1;

		// Search idle server
		int idle_offset = -1;
		for (int i = 0; i < NUMBER_OF_TICKET_OFFICE_SERVER; i++)
		{
			if (state->server_occupation[i] == 0)
			{
				idle_offset = i;
				break;
			}
		}

		if (get_random(2) <= P_LEAVE_TICKET_OFFICE)
		{
			struct queue_node *abandon_node = (struct queue_node *)malloc(sizeof(struct queue_node));
			if (!abandon_node)
			{
				printf("Error in malloc in user arrival in ticket office!\n");
				exit(-1);
			}

			abandon_node->id = loss->index_user;
			abandon_node->arrival_time = get_abandon_ticket_office(time->current);

			enqueue_node(&events->head_ticket_office, &events->tail_ticket_office, abandon_node);
		}
		else
		{
			if (idle_offset >= 0)
			{
				// Set idle server to busy server and update departure time
				state->server_occupation[idle_offset] = 1;
				events->completionTimes_ticket_office[idle_offset] = get_ticket_office_departure(time->current);
				state->server_count += 1;
			}
			else if (idle_offset == -1)
			{
				state->queue_count += 1;
			}
			state->population = state->queue_count + state->server_count;
		}
	}
}

void user_arrivals_ticket_office_feedback(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{

	events->user_arrival_to_ticket_office.user_arrival_time = get_user_arrival_to_ticket_office(time->current, rate);

	time->last[1] = time->current;

	loss->index_user += 1;

	// Search idle server
	int idle_offset = -1;
	for (int i = 0; i < NUMBER_OF_TICKET_OFFICE_SERVER; i++)
	{
		if (state->server_occupation[i] == 0)
		{
			idle_offset = i;
			break;
		}
	}

	if (get_random(3) <= P_LEAVE_TICKET_OFFICE)
	{
		struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
		if (!job)
		{
			printf("Error in malloc in user arrival in ticket office!\n");
			exit(-1);
		}
		job->id = loss->index_user;
		job->arrival_time = get_abandon_ticket_office(time->current);
		enqueue_node(&events->head_ticket_office, &events->tail_ticket_office, job);
	}
	else
	{
		if (idle_offset >= 0)
		{
			// Set idle server to busy server and update departure time
			state->server_occupation[idle_offset] = 1;
			events->completionTimes_ticket_office[idle_offset] = get_ticket_office_departure(time->current);
			state->server_count += 1;
		}
		else if (idle_offset == -1)
		{
			state->queue_count += 1;
		}
		state->population = state->queue_count + state->server_count;
	}
}

void user_departure_ticket_office(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset, double rate)
{
	state->server_count -= 1;

	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	if (state->queue_count > 0)
	{
		events->completionTimes_ticket_office[server_offset] = get_ticket_office_departure(time->current);
		state->server_occupation[server_offset] = 1;
		state->server_count += 1;
		state->queue_count -= 1;
	}
	else
	{
		events->completionTimes_ticket_office[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in ticket office departure!\n");
		exit(-1);
	}
	tail_job->id = loss->index_user;
	tail_job->arrival_time = time->current;
	enqueue_node(&events->head_ticket_purchased, &events->tail_ticket_purchased, tail_job);
	state->population = state->queue_count + state->server_count;
	routing_ticket_purchased(events, time, rate);
}

void abandon_ticket_office(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	struct queue_node *current = events->head_ticket_office;
	while (current != NULL)
	{
		if (current->id == job_id)
		{
			break;
		}
		current = current->next;
	}

	if (current == NULL)
	{
		printf("Job ID %d not found in the ticket office queue!\n", job_id);
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
		events->head_ticket_office = next;
	}

	if (next != NULL)
	{
		next->prev = prev;
	}
	else
	{
		events->tail_ticket_office = prev; // Aggiorna la coda quando rimuovi l'ultimo elemento
	}

	free(current);
	loss->loss_user += 1;
}
