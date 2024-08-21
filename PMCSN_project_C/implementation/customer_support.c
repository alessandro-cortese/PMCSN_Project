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
	// printf("Arrivo customer support\n");
	// printf("state->queue_count = %d\n", state->queue_count);
	// printf("state->server_count = %d\n", state->server_count);

	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in user arrival in customer support!\n");
		exit(-1);
	}

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
		printf("1.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
		// abandon is here
		printf("Random choose abandon customer support!\n");
		struct queue_node *abandon_job = (struct queue_node *)malloc(sizeof(struct queue_node));
		if (!abandon_job)
		{
			printf("Error in malloc in user arrival in customer support!\n");
			exit(-1);
		}

		// CASE 2: delete a node from head_ticket_purchased and add to abandon customer support queue
		abandon_job = (struct abandon_node *)events->head_queue_customer_support;
		events->head_queue_customer_support = tail_job->next;

		abandon_job->id = loss->index_user;
		abandon_job->arrival_time = get_abandon_customer_support(time->current);

		printf("2.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));

		if (events->head_customer_support == NULL)
		{
			printf("4.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
			events->head_customer_support = abandon_job;
			events->head_customer_support->prev = NULL;
			events->head_customer_support->next = NULL;
			printf("5.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
			events->tail_customer_support = abandon_job;
		}
		else if (events->head_customer_support != NULL)
		{
			printf("6.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
			events->tail_customer_support->next = abandon_job;
			printf("8.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
			abandon_job->prev = events->tail_customer_support;
			printf("9.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
			abandon_job->next = NULL;
			printf("10.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
			events->tail_customer_support = abandon_job;
			printf("7.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
		}

		printf("3.lenOfQueue(events->head_ticket_gate) = %d\n", lenOfQueue(events->head_ticket_gate));
		abandon_job = NULL;
	}
	else
	{
		printf("idle offset for customer support is %d\n", idle_offset);
		if (idle_offset >= 0 && events->head_queue_customer_support != NULL)
		{
			state->server_count++;
			state->server_occupation[idle_offset] = 1;
			events->completionTimes_customer_support[idle_offset] = get_customer_support_departure(time->current);

			processed_job_customer_support[idle_offset] = events->head_queue_customer_support->id;
			events->head_queue_customer_support = events->head_queue_customer_support->next;

			tail_job->id = loss->index_user;
			events->tail_queue_customer_support->next = tail_job;
			tail_job->prev = events->tail_queue_customer_support;
			tail_job->next = NULL;
			events->tail_queue_customer_support = tail_job;
			time->last[2] = time->current;
			tail_job = NULL;
		}
		else if (idle_offset == -1)
		{
			state->queue_count++;
		}
		state->population = state->queue_count + state->server_count;
	}

	// printf("Arrivo dopo customer support\n");
	// printf("state->queue_count = %d\n", state->queue_count);
	// printf("state->server_count = %d\n", state->server_count);
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
	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	// printf("events->head_queue_customer_support->id %d\n", events->head_queue_customer_support->id);
	if (state->queue_count > 0)
	{
		events->completionTimes_customer_support[server_offset] = get_customer_support_departure(time->current);
		state->server_occupation[server_offset] = 1;
		tail_job->id = processed_job_customer_support[server_offset];
		tail_job->arrival_time = events->completionTimes_customer_support[server_offset];
		state->queue_count--;
		state->server_count++;
		time->last[2] = time->current;
	}
	else
	{
		events->completionTimes_customer_support[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	// feedback is here
	if (get_random(9) <= P_OF_CHANGE_TICKET)
	{
		printf("Feedback event\n");
		feedback(events, time, rate);
	}
	else
	{
		if (events->head_user_to_security_check == NULL)
		{
			events->head_user_to_security_check = tail_job;
			events->tail_user_to_security_check = tail_job;
			tail_job->prev = NULL;
			tail_job->next = NULL;
		}
		else
		{
			events->tail_user_to_security_check->next = tail_job;
			tail_job->prev = events->tail_user_to_security_check;
			tail_job->next = NULL;
			events->tail_user_to_security_check = tail_job;
		}
		tail_job = NULL;
		routing_security_check(events, time, rate);
	}
	state->population = state->queue_count + state->server_count;
}

void abandon_customer_support(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	printf("Abandon customer support!\n");
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
}