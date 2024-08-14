#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/ticket_office.h"
#include "../data_structures/event_list.h"
#include "../headers/utility_functions.h"

double get_user_arrival_to_ticket_office(double arrival, double rate)
{
	SelectStream(3);
	arrival += Exponential(rate / (P_TICKET_NOT_PURCHASED * P_TICKET_PURCHASED_FROM_TICKET_OFFICE));
	return (arrival);
}

double get_ticket_office_departure(double start)
{
	SelectStream(5);
	double departure = start + Exponential(SR_TICKET_OFFICE_OPERATOR);
	return departure;
}

void user_arrivals_ticket_office(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{
	events->user_arrival_to_ticket_office.user_arrival_time = get_user_arrival_to_ticket_office(time->current, rate);

	time->last[1] = time->current;

	if (events->user_arrival_to_ticket_office.user_arrival_time > STOP)
	{
		events->user_arrival_to_ticket_office.user_arrival_time = (double)INFINITY;
		events->user_arrival_to_ticket_office.is_user_arrival_active = false;
		printf("Stop arrival to ticket office!\n");
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

		if (Random() <= P_LEAVE_TICKET_OFFICE)
		{
			struct abandon_node *tail_job = (struct abandon_node *)malloc(sizeof(struct abandon_node));
			if (!tail_job)
			{
				printf("Error in malloc in user arrival in ticket office!\n");
				exit(-1);
			}
			tail_job->id = loss->index_user;
			printf("abandon job id is %d\n", tail_job->id);
			tail_job->abandon_time = time->current;
			printf("abandon time is %f\n", tail_job->abandon_time);

			// If is the first time that a job abandon the queue
			if (events->head_ticket_office == NULL)
			{
				events->head_ticket_office = tail_job;
				events->tail_ticket_office = tail_job;
				tail_job->prev = NULL;
				tail_job->next = NULL;
			}
			else
			{
				events->tail_ticket_office->next = tail_job;
				tail_job->prev = events->tail_ticket_office;
				tail_job->next = NULL;
				events->tail_ticket_office = tail_job;
			}
			tail_job = NULL;
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
				state->queue_count += +1;
			}
			state->population = state->queue_count + state->server_count;
		}
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
		state->server_occupation[server_offset] = 0;
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

	if (events->head_ticket_purchased == NULL)
	{
		events->head_ticket_purchased = tail_job;
		events->head_ticket_purchased->prev = NULL;
		events->head_ticket_purchased->next = NULL;
		events->tail_ticket_purchased = tail_job;
	}
	else if (events->head_ticket_purchased != NULL)
	{
		events->tail_ticket_purchased->next = tail_job;
		tail_job->prev = events->tail_ticket_purchased;
		tail_job->next = NULL;
		events->tail_ticket_purchased = tail_job;
	}
	tail_job = NULL;
	state->population = state->queue_count + state->server_count;
	routing_ticket_purchased(events, time, rate);
}

void abandon_ticket_office(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	printf("Abandon ticket office!\n");
	if (events->head_ticket_office != NULL)
	{
		struct abandon_node *current = events->head_ticket_office;
		while (current != NULL)
		{
			if (current->id == job_id)
			{
				printf("current->id = %d\n", current->id);
				break;
			}
			current = current->next;
		}

		struct abandon_node *prev = current->prev;
		struct abandon_node *next = current->next;

		if (prev != NULL)
		{
			prev->next = current->next;
		}
		else
		{
			events->head_ticket_office = next;
		}

		if (next != NULL)
		{
			next->prev = current->prev;
		}
		else
		{
			events->head_ticket_office = prev;
		}

		free(current);
		loss->loss_user += 1;
	}
}