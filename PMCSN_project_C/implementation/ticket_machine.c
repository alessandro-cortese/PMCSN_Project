#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/ticket_machine.h"
#include "../headers/rngs.h"
#include "../data_structures/event_list.h"

double get_user_arrival_to_ticket_machine(double arrival, double rate)
{
	/*
	if (Random() <= (P_TICKET_NOT_PURCHASED * P_TICKET_PURCHASED_FROM_TICKET_STATION))
	{
		printf("Rand pick ticket machine stream\n");
		SelectStream(2);
		arrival += Exponential(rate / (P_TICKET_PURCHASED_FROM_TICKET_STATION));
		return (arrival);
	}else{
		if(arrival != 0.0)
			return arrival;
	}
	*/
	SelectStream(2);
	arrival += Exponential(rate / (P_TICKET_PURCHASED_FROM_TICKET_STATION));
	return (arrival);
	
}
double get_first_arrival_to_ticket_machine(double arrival, double rate){
	if (Random() <= (P_TICKET_NOT_PURCHASED * P_TICKET_PURCHASED_FROM_TICKET_STATION))
	{
		printf("Rand pick ticket machine stream\n");
		SelectStream(2);
		arrival += Exponential(rate / (P_TICKET_PURCHASED_FROM_TICKET_STATION));
		return (arrival);
	}
}
double get_ticket_machine_departure(double start)
{
	SelectStream(4);
	double departure = start + Exponential(SR_TICKET_STATION);
	return departure;
}

// To undestand why exists...
//  double get_abandon_ticket_machine(double arrival)
//  {
//  	SelectStream(9);
//  	double abandon = arrival + Exponential(P_LEAVE_TICKET_STATION);
//  	return abandon;
//  }

void user_arrivals_ticket_machine(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{
	loss->index_user += 1;
	state->population += 1;

	// generate next event
	events->user_arrival_to_ticket_machine.user_arrival_time = get_user_arrival_to_ticket_machine(time->current, rate);
	printf("Dentro user arrival ticket machine: %f\n", events->user_arrival_to_ticket_machine.user_arrival_time);

	time->last[0] = time->current;

	if (events->user_arrival_to_ticket_machine.user_arrival_time > STOP)
	{
		events->user_arrival_to_ticket_machine.user_arrival_time = (double)INFINITY;
		events->user_arrival_to_ticket_machine.is_user_arrival_active = false;
		printf("events->user_arrival_to_ticket_machine.is_user_arrival_active = %d\n", events->user_arrival_to_ticket_machine.is_user_arrival_active);
	}

	// Search idle server
	int idle_offset = -1;
	for (int i = 0; i < NUMBER_OF_TICKET_MACHINE_SERVER; i++)
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
		events->completionTimes_ticket_machine[idle_offset] = get_ticket_machine_departure(time->current);
		// Prendo il job che sta in testa e lo processo
	}

	if (Random() <= P_LEAVE_TICKET_STATION)
	{
		struct abandon_node *tail_job = (struct abandon_node *)malloc(sizeof(struct abandon_node));
		if (!tail_job)
		{
			printf("Error in malloc in user arrival in ticket machine!\n");
			exit(-1);
		}
		tail_job->id = loss->index_user;
		tail_job->abandon_time = time->current;
		tail_job->next = NULL;
		tail_job->prev = events->tail_ticket_machine;

		// If the first time that a job adandon the queue
		if (events->head_ticket_machine == NULL)
		{
			events->head_ticket_machine = tail_job;
			events->tail_ticket_machine = tail_job;
		}

		free(tail_job);
	}
}

void user_departure_ticket_machine(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	state->population -= 1;

	// TODO: per il modello migliorativo considerare una coda con priorità

	// If the population is bigger of 0 then update server completion time,
	// otherwise reset data of the server.
	if (state->population > 0)
	{
		events->completionTimes_ticket_machine[server_offset] = get_ticket_machine_departure(time->current);
		state->server_occupation[server_offset] = 0;
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
	free(tail_job);
}

void abandon_ticket_machine(struct event_list *events, struct states *state, struct loss *loss, int job_id)
{
	if (events->head_ticket_machine != NULL)
	{
		struct abandon_node *current = events->head_ticket_machine;
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
		state->population -= 1;
	}
}