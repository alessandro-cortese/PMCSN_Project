#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/customer_support.h"
#include "../headers/ticket_office.h"
#include "../data_structures/event_list.h"

int processed_job_customer_support[NUMBER_OF_CUSTOMER_SUPPORT_SERVER];

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
	// TODO: Define a function with different stream
	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in user arrival in customer support!\n");
		exit(-1);
	}

	if (Random() <= P_OF_CUSTOMER_SUPPORT)
	{

		loss->index_user += 1;
		state->population += 1;
		events->user_arrival_to_customer_support.user_arrival_time = events->head_ticket_purchased->arrival_time;

		time->last[2] = time->current;

		if (events->user_arrival_to_customer_support.user_arrival_time > STOP)
		{
			events->user_arrival_to_customer_support.user_arrival_time = (double)INFINITY;
			events->user_arrival_to_customer_support.is_user_arrival_active = false;
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

		if (idle_offset >= 0 && events->head_queue_customer_support == NULL)
		{
			// CASE 0: change head ticket purchased and store job id into array
			state->server_occupation[idle_offset] = 1;
			events->completionTimes_customer_support[idle_offset] = get_customer_support_departure(time->current);
			processed_job_customer_support[idle_offset] = events->head_ticket_purchased->id;
			events->head_ticket_purchased = events->head_ticket_purchased->next;
		}
		else if (idle_offset >= 0 && events->head_queue_customer_support != NULL)
		{
			// delete from customer support queue
			state->server_occupation[idle_offset] = 1;
			events->completionTimes_customer_support[idle_offset] = get_customer_support_departure(time->current);
			processed_job_customer_support[idle_offset] = events->head_queue_customer_support->id;
			events->head_queue_customer_support = events->head_queue_customer_support->next;
			tail_job->id = loss->index_user;
			events->tail_queue_customer_support->next = tail_job;
			tail_job->prev = events->tail_queue_customer_support;
			tail_job->next = NULL;
			events->tail_queue_customer_support = tail_job;
			free(tail_job);
		}

		if (idle_offset == -1)
		{
			// CASE 1: delete a node from head_ticket_purchased and add to customer support queue
			tail_job = events->head_ticket_purchased;
			events->head_ticket_purchased = tail_job->next;

			if (events->head_queue_customer_support == NULL)
			{
				events->head_queue_customer_support = tail_job;
				events->head_queue_customer_support->next = NULL;
				events->head_queue_customer_support->prev = NULL;
				events->tail_queue_customer_support = tail_job;
				free(tail_job);
			}
			else
			{
				events->tail_queue_customer_support->next = tail_job;
				tail_job->next = NULL;
				tail_job->prev = events->tail_queue_customer_support;
				events->tail_queue_customer_support = tail_job;
				free(tail_job);
			}
		}

		if (Random() <= P_LEAVE_CUSTOMER_SUPPORT)
		{

			struct abandon_node *abandon_job = (struct abandon_node *)malloc(sizeof(struct abandon_node));
			if (!abandon_job)
			{
				printf("Error in malloc in user arrival in customer support!\n");
				exit(-1);
			}

			// CASE 2: delete a node from head_ticket_purchased and add to abandon customer support queue
			abandon_job = (struct abandon_node *)events->head_ticket_purchased;
			events->head_ticket_purchased = tail_job->next;

			abandon_job->id = loss->index_user;
			abandon_job->abandon_time = time->current;

			if (events->head_customer_support == NULL)
			{
				events->head_customer_support = abandon_job;
				events->head_customer_support->prev = NULL;
				events->head_customer_support->next = NULL;
				events->tail_customer_support = abandon_job;
			}
			else if (events->head_customer_support != NULL)
			{
				events->tail_customer_support->next = abandon_job;
				abandon_job->prev = events->tail_customer_support;
				abandon_job->next = NULL;
				events->tail_customer_support = abandon_job;
			}
			free(abandon_job);
		}
	}
	else
	{
		// CASE 3: skip customer support {remove node from ticket purchased and add a node into head security check queue}
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

	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in user departure in customer support!\n");
		exit(-1);
	}
	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	if (state->population > 0)
	{
		events->completionTimes_customer_support[server_offset] = get_customer_support_departure(time->current);
		state->server_occupation[server_offset] = 0;
		tail_job->id = processed_job_customer_support[server_offset];
	}
	else
	{
		events->completionTimes_customer_support[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	// feedback is here
	if (Random() <= P_OF_CHANGE_TICKET)
	{
		state->server_occupation[server_offset] = 0;
		user_arrivals_ticket_office(events, time, state, loss, rate);
	}
	else
	{
		tail_job->arrival_time = time->current;

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
	struct abandon_node *current = events->head_customer_support;
	while (current != NULL)
	{
		if (current->id == job_id)
			break;
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