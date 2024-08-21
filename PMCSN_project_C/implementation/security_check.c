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

	// state->population += 1;

	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in user arrival in security check!\n");
		exit(-1);
	}

	loss->index_user += 1;

	time->last[3] = time->current;

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
		state->server_count++;
		state->server_occupation[idle_offset] = 1;
		events->completionTimes_security_check[idle_offset] = get_security_check_departure(time->current);

		processed_job_security_check[idle_offset] = events->head_security_check_queue->id;
		events->head_security_check_queue = events->head_security_check_queue->next;

		tail_job->id = loss->index_user;
		events->tail_security_check_queue->next = tail_job;
		tail_job->prev = events->tail_security_check_queue;
		tail_job->next = NULL;
		events->tail_security_check_queue = tail_job;

		tail_job = NULL;
	}
	else if (idle_offset == -1)
	{
		state->queue_count++;
	}

	state->population = state->queue_count + state->server_count;
}

void user_departure_security_check(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	state->server_count--;

	// Inserimento in coda di un nuovo nodo all'interno della lista degli arrivi al ticket gate
	struct queue_node *tail = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail)
	{
		printf("Error in malloc in user departure in security check!\n");
		exit(-1);
	}
	if (!(get_random(11) <= P_LEAVE_SECURITY_CONTROL))
	{

		printf("prima\n");
		tail->id = processed_job_security_check[server_offset];
		tail->arrival_time = time->current;

		if (events->head_ticket_gate == NULL)
		{
			tail->id = processed_job_security_check[server_offset];
			events->head_ticket_gate = tail;
			events->head_ticket_gate->prev = NULL;
			events->head_ticket_gate->next = NULL;
			events->tail_ticket_gate = tail;
		}
		else if (events->head_ticket_gate != NULL)
		{
			tail->id = processed_job_security_check[server_offset];
			events->tail_ticket_gate->next = tail;
			tail->prev = events->tail_ticket_gate;
			tail->next = NULL;
			events->tail_ticket_gate = tail;
		}

		tail = NULL;
		routing_ticket_gate(events, time);
	}
	else
	{
		printf("Job have false document\n");
		// prob of having false document or other problems
		events->completionTimes_security_check[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	if (state->queue_count > 0)
	{
		processed_job_security_check[server_offset] = events->head_security_check_queue->id;
		state->server_occupation[server_offset] = 1;
		events->completionTimes_security_check[server_offset] = get_security_check_departure(time->current);
		events->head_security_check_queue = events->head_security_check_queue->next;
		state->queue_count--;
		state->server_count++;
	}
	else
	{
		events->completionTimes_security_check[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	state->population = state->queue_count + state->server_count;
}