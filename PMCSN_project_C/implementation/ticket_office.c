#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/ticket_office.h"

double get_user_arrival_to_ticket_office(double arrival, double rate)
{
	SelectStream(3);
	arrival += Exponential(rate * (P_TICKET_PURCHASED_FROM_TICKET_OFFICE * P_TICKET_NOT_PURCHASED));
	return (arrival);
}

double get_ticket_office_departure(double start)
{
	SelectStream(5);
	double departure = start + Exponential(SR_TICKET_OFFICE_OPERATOR);
	return departure;
}

// TODO: understand why exist
// double get_abandon_ticket_office(double arrival)
//{
//	SelectStream(10);
//	double abandon = arrival + Exponential(P_LEAVE_TICKET_OFFICE);
//	return abandon;
// }

void user_arrivals_ticket_office(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{
	loss->index_user += 1;
	state->population += 1;
	events->user_arrival_to_ticket_office.user_arrival_time = get_user_arrival_to_ticket_office(time->current, rate);

	if (events->user_arrival_to_ticket_office.user_arrival_time > STOP)
	{
		events->user_arrival_to_ticket_office.user_arrival_time = (double)INFINITY;
		events->user_arrival_to_ticket_office.is_user_arrival_active = false;
		time->last[0] = time->current;
	}

	// Search idle server
	int idle_offset = -1;
	for (int i = 0; i < NUMBER_OF_TICKET_OFFICE_SERVER; i++)
	{
		if (state->server_occupation[i] == 0)
		{
			idle_offset == i;
			break;
		}
	}

	if (idle_offset >= 0)
	{
		// Set idle server to busy server and update departure time
		state->server_occupation[idle_offset] = 1;
		events->completionTimes_ticket_office[idle_offset] = get_ticket_office_departure(time->current);
	}
	else if (Random() <= P_LEAVE_TICKET_OFFICE)
	{
		struct user *tail_job = (struct user *)malloc(sizeof(struct user));
		if (!tail_job)
		{
			printf("Error in malloc in user arrival in ticket office!\n");
			exit(-1);
		}
		tail_job->id = loss->index_user;
		tail_job->abandonTime = time->current;
		tail_job->next = NULL;
		tail_job->prev = events->tail_ticket_office;

		abandon_ticket_office(events, state, loss, tail_job->id);
		free(tail_job);
	}
}

void user_departure_ticket_office(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	state->population -= 1;

	// Remove of head node from list of dropout
	if (events->head_ticket_office != NULL)
	{
		struct user *user_to_remove = events->head_ticket_office;
		if (user_to_remove->next == NULL)
		{
			events->head_ticket_office = NULL;
			events->tail_ticket_office = NULL;
		}
		else
		{
			events->head_ticket_office = user_to_remove->next;
			events->head_ticket_office->prev = NULL;
		}
		free(user_to_remove);
	}

	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	if (state->population > 0)
	{
		events->completionTimes_ticket_office[server_offset] = get_ticket_office_departure(time->current);
	}
	else
	{
		events->completionTimes_ticket_office[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	struct user *tail_job = (struct user *)malloc(sizeof(struct user));
	if (!tail_job)
	{
		printf("Error in malloc in ticket office departure!\n");
		exit(-1);
	}
	tail_job->id = loss->index_user;
	tail_job->abandonTime = get_ticket_office_departure(time->current);

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
}

void abandon_ticket_office(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	struct user *current = events->head_ticket_office;
	while (current != NULL)
	{
		if (current->id == job_id)
			break;
		current = current->next;
	}

	struct user *prev = current->prev;
	struct user *next = current->next;

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
	state->population -= 1;
}