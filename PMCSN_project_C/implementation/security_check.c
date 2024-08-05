#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/security_check.h"

// TODO: we have to understad why exist...
// double get_user_arrival_to_security_check(double arrival)
// {
// 	// TODO: bisogna capire come riunire i flussi
// 	//		e aggiungere la probabilità
// 	return (arrival);
// }

double get_security_check_departure(double start)
{
	SelectStream(8);
	double departure = start + Exponential(SR_SECURITY_CONTROL_OPERATOR);
	return departure;
}

// TODO: we have to understand why exists
// double get_abandon_security_check(double arrival)
// {
// 	SelectStream(13);
// 	double abandon = arrival + Exponential(P_LEAVE_SECURITY_CONTROL);
// 	return abandon;
// }

void user_arrivals_security_check(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{
	struct user *tail_job = (struct user *)malloc(sizeof(struct user));
	if (!tail_job)
	{
		printf("Error in malloc in user arrival in security check!\n");
		exit(-1);
	}
	if (Random() <= P_OF_SECURITY_CHECK)
	{
		loss->index_user += 1;
		state->population += 1;
		events->user_arrival_to_security_check.user_arrival_time = get_security_check_departure(rate);

		if (events->user_arrival_to_security_check.user_arrival_time > STOP)
		{
			events->user_arrival_to_security_check.is_user_arrival_active = (double)INFINITY;
			events->user_arrival_to_security_check.is_user_arrival_active = false;
			time->last[3] = time->current;
		}

		int idle_offset = -1;
		for (int i = 0; i < NUMBER_OF_SECURITY_CHECK_SERVERS; i++)
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
			events->completionTimes_security_check[idle_offset] = get_security_check_departure(time->current);
		}
	}
	else
	{
		if (events->head_ticket_gate == NULL)
		{
			events->head_ticket_gate = tail_job;
			events->head_ticket_gate->prev = NULL;
			events->head_ticket_gate->next = NULL;
			events->head_ticket_gate = tail_job;
		}
		else if (events->head_ticket_gate != NULL)
		{
			events->head_ticket_gate->next = tail_job;
			tail_job->prev = events->tail_ticket_gate;
			tail_job->next = NULL;
			events->tail_ticket_gate = tail_job;
		}
	}
}

void user_departure_security_check(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	state->population -= 1;

	if (events->head_security_check != NULL)
	{
		struct user *user_to_remove = events->head_security_check;
		if (user_to_remove->next == NULL)
		{
			events->head_security_check = NULL;
			events->tail_security_check = NULL;
		}
		else
		{
			events->head_security_check = user_to_remove->next;
			events->head_security_check->prev = NULL;
		}
		free(user_to_remove);
	}

	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	if (state->population > 0)
	{
		events->completionTimes_security_check[server_offset] = get_security_check_departure(time->current);
	}
	else
	{
		events->completionTimes_security_check[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	// Inserimento in coda di un nuovo nodo all'interno della lista degli arrivi al ticket gate
	struct user *tail = (struct user *)malloc(sizeof(struct user));
	if (!tail)
	{
		printf("Error in malloc in user departure in security check!\n");
		exit(-1);
	}
	if (!(Random() <= P_LEAVE_SECURITY_CONTROL))
	{
		tail->id = loss->index_user;
		tail->abandonTime = get_security_check_departure(time->current);

		if (events->head_ticket_gate == NULL)
		{
			events->head_ticket_gate = tail;
			events->head_ticket_gate->prev = NULL;
			events->head_ticket_gate->next = NULL;
			events->head_ticket_gate = tail;
		}
		else if (events->head_ticket_gate != NULL)
		{
			events->head_ticket_gate->next = tail;
			tail->prev = events->tail_ticket_gate;
			tail->next = NULL;
			events->tail_customer_support = tail;
		}
		free(tail);
	}
	else
	{
		//Uscita dal sistema
	}
}
void abandon_security_check(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	struct user *current = events->head_security_check;
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
		events->head_security_check = next;
	}

	if (next != NULL)
	{
		next->prev = current->prev;
	}
	else
	{
		events->head_security_check = prev;
	}

	free(current);
	loss->loss_user += 1;
	state->population -= 1;
}