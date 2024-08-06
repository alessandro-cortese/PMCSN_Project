#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/customer_support.h"
#include "../headers/ticket_office.h"

// we don't need to model the stream cause it's the previous one
//  double get_user_arrival_to_customer_support(double arrival)
//  {
//  	// TODO: bisogna capire come riunire i flussi
//  	//		e aggiungere la probabilità
//  	return (arrival);
//  }

double get_customer_support_departure(double start)
{
	SelectStream(7);
	double departure = start + Exponential(SR_CUSTOMER_SUPPORT_OPERATOR);
	return departure;
}

// TODO: understand why exist...
// double get_abandon_customer_support(double arrival)
// {
// 	SelectStream(12);
// 	double abandon = arrival + Exponential(P_LEAVE_CUSTOMER_SUPPORT);
// 	return abandon;
// }

void user_arrivals_customer_support(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{
	// Alg:
	//  1. prendere quello di testa
	//  2. calcolare la probabilità
	//		3.1 va nella nuova lista per il controllo sicurezza
	//		3.2 altrimnti deve essere gestito dall'assistenza clienti

	// TODO: Define a function with different stream
	struct user *tail_job = (struct user *)malloc(sizeof(struct user));
	if (!tail_job)
	{
		printf("Error in malloc in user arrival in customer support!\n");
		exit(-1);
	}
	if (Random() <= P_OF_CUSTOMER_SUPPORT)
	{

		loss->index_user += 1;
		state->population += 1;
		events->user_arrival_to_customer_support.user_arrival_time = get_user_arrival_to_customer_support(rate);

		if (events->user_arrival_to_customer_support.user_arrival_time > STOP)
		{
			events->user_arrival_to_customer_support.user_arrival_time = (double)INFINITY;
			events->user_arrival_to_customer_support.is_user_arrival_active = false;
			time->last[2] = time->current;
		}

		int idle_offset = -1;
		for (int i = 0; i < NUMBER_OF_CUSTOMER_SUPPORT_SERVER; i++)
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
			events->completionTimes_customer_support[idle_offset] = get_customer_support_departure(time->current);
		}

		else if (Random() <= P_LEAVE_CUSTOMER_SUPPORT)
		{
			tail_job->id = loss->index_user;
			tail_job->abandonTime = time->current;
			tail_job->next = NULL;
			tail_job->prev = events->tail_ticket_machine;

			free(tail_job);
		}
	}
	else
	{
		// CASE 2: skip customer support {remove node from ticket purchased and add a node into head security check queue}
		tail_job = events->head_ticket_purchased;
		events->head_ticket_purchased = tail_job->next;
		tail_job->next = NULL;
		tail_job->prev = NULL;

		if (events->head_user_to_security_check == NULL)
		{
			events->head_user_to_security_check = tail_job;
			events->head_user_to_security_check->prev = NULL;
			events->head_user_to_security_check->next = NULL;
			events->tail_user_to_security_check = tail_job;
		}
		else if (events->head_user_to_security_check != NULL)
		{
			events->head_user_to_security_check->next = tail_job;
			tail_job->prev = events->tail_user_to_security_check;
			tail_job->next = NULL;
			events->tail_user_to_security_check = tail_job;
		}
		free(tail_job);
	}
}

void user_departure_customer_support(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset, double rate)
{
	state->population -= 1;

	if (events->head_customer_support != NULL)
	{
		struct user *user_to_remove = events->head_customer_support;
		if (user_to_remove->next == NULL)
		{
			events->head_customer_support = NULL;
			events->tail_customer_support = NULL;
		}
		else
		{
			events->head_customer_support = user_to_remove->next;
			events->head_customer_support->prev = NULL;
		}
		free(user_to_remove);
	}

	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	if (state->population > 0)
	{
		events->completionTimes_customer_support[server_offset] = get_customer_support_departure(time->current);
	}
	else
	{
		events->completionTimes_customer_support[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	// feedback is here
	if (Random() <= P_OF_CHANGE_TICKET)
	{
		user_arrivals_ticket_office(events, time, state, loss, rate);
	}
	else
	{
		struct user *tail_job = (struct user *)malloc(sizeof(struct user));
		if (!tail_job)
		{
			printf("Error in malloc in customer support departure!\n");
			exit(-1);
		}

		tail_job->id = loss->index_user;
		tail_job->abandonTime = get_customer_support_departure(time->current);

		if (events->head_user_to_security_check == NULL)
		{
			events->head_user_to_security_check = tail_job;
			events->head_user_to_security_check->prev = NULL;
			events->head_user_to_security_check->next = NULL;
			events->tail_user_to_security_check = tail_job;
		}
		else if (events->head_user_to_security_check != NULL)
		{
			events->head_user_to_security_check->next = tail_job;
			tail_job->prev = events->tail_user_to_security_check;
			tail_job->next = NULL;
			events->tail_user_to_security_check = tail_job;
		}
		free(tail_job);
	}
}

void abandon_customer_support(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	struct user *current = events->head_customer_support;
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
		events->head_customer_support = next;
	}

	if (next != NULL)
	{
		next->prev = current->prev;
	}
	else
	{
		events->head_customer_support = prev;
	}

	free(current);
	loss->loss_user += 1;
	state->population -= 1;
}