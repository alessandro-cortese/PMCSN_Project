#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/ticket_machine.h"
#include "../headers/rngs.h"
#include "../data_structures/event_list.h"
#include "../headers/utility_functions.h"

double get_user_arrival_to_ticket_machine(double arrival, double rate)
{
	SelectStream(4);
	arrival += Exponential(rate / P_TICKET_PURCHASED_FROM_TICKET_STATION);
	return (arrival);
}

double get_ticket_machine_departure(double start)
{
	SelectStream(5);
	double departure = start + Exponential(SR_TICKET_STATION);
	return departure;
}

double get_abandon_ticket_machine(double start)
{
	SelectStream(16);
	double departure = start + Exponential(360);
	return departure;
}

void user_arrivals_ticket_machine(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{

	// printf("Evento di arrivo in ticket machine!\n");
	// printf("state->queue_count = %d\n", state->queue_count);
	// printf("state->server_count = %d\n", state->server_count);

	// generate next event
	events->user_arrival_to_ticket_machine.user_arrival_time = get_user_arrival_to_ticket_machine(time->current, rate);

	time->last[0] = time->current;

	if (events->user_arrival_to_ticket_machine.user_arrival_time > STOP)
	{
		events->user_arrival_to_ticket_machine.user_arrival_time = (double)INFINITY;
		events->user_arrival_to_ticket_machine.is_user_arrival_active = false;
		printf("Stop arrival to ticket machine!\n");
	}
	else
	{

		loss->index_user += 1;

		// Search idle server
		int idle_offset = -1;
		for (int i = 0; i < NUMBER_OF_TICKET_MACHINE_SERVER; i++)
		{

			if (state->server_occupation[i] == 0)
			{
				idle_offset = i;
				break;
			}
		}

		if (get_random(6) <= P_LEAVE_TICKET_STATION)
		{
			struct queue_node *abandon_job = (struct queue_node *)malloc(sizeof(struct queue_node));
			if (!abandon_job)
			{
				printf("Error in malloc in user arrival in ticket machine!\n");
				exit(-1);
			}
			abandon_job->id = loss->index_user;
			printf("abandon job id is %d\n", abandon_job->id);
			abandon_job->arrival_time = get_abandon_ticket_machine(time->current);
			printf("abandon time is %f\n", abandon_job->arrival_time);
			// If is the first time that a job abandon the queue
			// if (events->head_ticket_machine == NULL)
			// {
			// 	events->head_ticket_machine = abandon_job;
			// 	events->tail_ticket_machine = abandon_job;
			// 	abandon_job->prev = NULL;
			// 	abandon_job->next = NULL;
			// }
			// else
			// {
			// 	events->tail_ticket_machine->next = abandon_job;
			// 	abandon_job->prev = events->tail_ticket_machine;
			// 	abandon_job->next = NULL;
			// 	events->tail_ticket_machine = abandon_job;
			// }

			// abandon_job = NULL;
			enqueue_node(&events->head_ticket_machine, &events->tail_ticket_machine, abandon_job);
		}
		else
		{
			if (idle_offset >= 0)
			{
				// Set idle server to busy server and update departure time
				state->server_occupation[idle_offset] = 1;
				events->completionTimes_ticket_machine[idle_offset] = get_ticket_machine_departure(time->current);
				// Prendo il job che sta in testa e lo processo
				// printf("events->completionTimes_ticket_machine[%d] = get_ticket_machine_departure(%f) = %f\n", idle_offset, time->current, events->completionTimes_ticket_machine[idle_offset] = get_ticket_machine_departure(time->current));
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

void user_departure_ticket_machine(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset, double rate)
{
	state->server_count -= 1;

	if (state->queue_count > 0)
	{
		events->completionTimes_ticket_machine[server_offset] = get_ticket_machine_departure(time->current);
		state->server_occupation[server_offset] = 1;
		state->server_count += 1;
		state->queue_count -= 1;
	}
	else
	{
		events->completionTimes_ticket_machine[server_offset] = (double)INFINITY;
		state->server_occupation[server_offset] = 0;
	}

	// Add a node into ticket purchased queue
	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in ticket machine departure!\n");
		exit(-1);
	}
	tail_job->id = loss->index_user;
	tail_job->arrival_time = events->completionTimes_ticket_machine[server_offset];

	// if (events->head_ticket_purchased == NULL)
	// {
	// 	events->head_ticket_purchased = tail_job;
	// 	events->head_ticket_purchased->prev = NULL;
	// 	events->head_ticket_purchased->next = NULL;
	// 	events->tail_ticket_purchased = tail_job;
	// }
	// else if (events->head_ticket_purchased != NULL)
	// {
	// 	events->tail_ticket_purchased->next = tail_job;
	// 	tail_job->prev = events->tail_ticket_purchased;
	// 	tail_job->next = NULL;
	// 	events->tail_ticket_purchased = tail_job;
	// }

	// tail_job = NULL;
	enqueue_node(&events->head_ticket_purchased, &events->tail_ticket_purchased, tail_job);
	state->population = state->queue_count + state->server_count;
	routing_ticket_purchased(events, time, rate);
}

void abandon_ticket_machine(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	printf("Abandon ticket machine!\n");
	if (events->head_ticket_machine != NULL)
	{
		struct queue_node *current = events->head_ticket_machine;
		while (current != NULL)
		{
			if (current->id == job_id)
			{
				printf("current->id = %d\n", current->id);
				break;
			}

			current = current->next;
		}

		struct queue_node *prev = current->prev;
		struct queue_node *next = current->next;

		if (prev != NULL)
		{
			prev->next = current->next;
		}
		else
		{
			events->head_ticket_machine = next;
		}

		if (next != NULL)
		{
			next->prev = current->prev;
		}
		else
		{
			events->head_ticket_machine = prev;
		}

		free(current);
		loss->loss_user += 1;
	}
}