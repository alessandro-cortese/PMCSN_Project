#include "../data_structures/event_list.h"
#include "../data_structures/states.h"
#include "../data_structures/area.h"
#include "../data_structures/time.h"
#include "../data_structures/loss.h"
#include "../data_structures/utils.h"
#include "../headers/ticket_machine.h"
#include "../headers/ticket_office.h"
#include "../headers/customer_support.h"
#include "../headers/security_check.h"
#include "../headers/ticket_gate.h"
#include "../headers/constants.h"
#include "../headers/rngs.h"
#include "../headers/utility_functions.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

double get_ticket_gate_departure(double start)
{
	SelectStream(11);
	double departure = start + Exponential(SR_TICKET_GATE_OPERATOR);
	return departure;
}

void user_arrivals_ticket_gate(struct event_list *events, struct time *time, struct states *state, struct loss *loss, double rate)
{
	printf("Dentro user arrival ticket gate\n");
	int idle_offset = -1;
	for (int i = 0; i < NUMBER_OF_TICKET_GATE_SERVERS; i++)
	{
		if (state->server_occupation[i] == 0)
		{
			idle_offset = i;
			break;
		}
	}

	printf("idle offset: %d\n", idle_offset);

	loss->index_user += 1;
	state->population += 1;

	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in user arrival in ticket gate!\n");
		exit(-1);
	}

	tail_job->id = loss->index_user;

	if (idle_offset >= 0 && events->head_ticket_gate != NULL)
	{

		// Set idle server to busy server and update departure time
		state->server_occupation[idle_offset] = 1;
		events->completionTimes_ticket_gate[idle_offset] = get_ticket_gate_departure(time->current);

		time->last[4] = time->current;

		tail_job = events->head_ticket_gate;
		events->head_ticket_gate = events->head_ticket_gate->next;
		tail_job->prev = NULL;
		tail_job->next = NULL;
		free(tail_job);

		// if (events->head_ticket_gate == events->tail_ticket_gate)
		// {
		// 	events->head_ticket_gate = NULL;
		// 	free(events->tail_ticket_gate);
		// 	events->tail_ticket_gate = NULL;
		// 	events->user_arrival_to_ticket_gate.user_arrival_time = (double)INFINITY;
		// }
		// else
		// {
		// 	tail_job = events->head_ticket_gate;
		// 	events->head_ticket_gate = tail_job->next;
		// 	tail_job->next = NULL;
		// 	tail_job->prev = NULL;
		// 	tail_job = NULL;
		// 	printf("events->user_arrival_to_ticket_gate.user_arrival_time = %f\n", events->user_arrival_to_ticket_gate.user_arrival_time);
		// 	printf("events->head_ticket_gate->arrival_time = %f\n", events->head_ticket_gate->arrival_time);
		// 	events->user_arrival_to_ticket_gate.user_arrival_time = events->head_ticket_gate->arrival_time;
		// }
	}
	else if (idle_offset == -1)
	{
		events->tail_ticket_gate->next = tail_job;
		tail_job->prev = events->tail_ticket_gate;
		tail_job->next = NULL;
		events->tail_ticket_gate = tail_job;
	}
}
void user_departure_ticket_gate(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	state->population -= 1;
	events->completionTimes_ticket_gate[server_offset] = (double)INFINITY;
	printf("events->completionTimes_ticket_gate[%d] %f\n", server_offset, events->completionTimes_ticket_gate[server_offset]);
	state->server_occupation[server_offset] = 0;
}
