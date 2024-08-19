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

void user_arrivals_ticket_gate(struct event_list *events, struct time *time, struct states *state, struct loss *loss)
{

	state->queue_count++;

	int idle_offset = -1;
	for (int i = 0; i < NUMBER_OF_TICKET_GATE_SERVERS; i++)
	{
		if (state->server_occupation[i] == 0)
		{
			idle_offset = i;
			break;
		}
	}

	loss->index_user += 1;

	struct queue_node *tail_job = (struct queue_node *)malloc(sizeof(struct queue_node));
	if (!tail_job)
	{
		printf("Error in malloc in user arrival in ticket gate!\n");
		exit(-1);
	}

	tail_job->id = loss->index_user;

	if (idle_offset >= 0 && events->head_ticket_gate != NULL)
	{
		state->server_count++;
		state->queue_count--;
		tail_job = events->head_ticket_gate;
		events->head_ticket_gate = events->head_ticket_gate->next;
		tail_job->prev = NULL;
		tail_job->next = NULL;
		// Set idle server to busy server and update departure time
		state->server_occupation[idle_offset] = 1;

		events->completionTimes_ticket_gate[idle_offset] = get_ticket_gate_departure(time->current);
		time->last[4] = time->current;

		free(tail_job);
	}

	state->population = state->queue_count + state->server_count;
	printf("Evento di arrivo in ticket gate!\n");
	printf("state->queue_count = %d\n", state->queue_count);
	printf("state->server_count = %d\n", state->server_count);
}
void user_departure_ticket_gate(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	state->server_count--;

	//if (events->head_ticket_gate != NULL)
	//	printf("events->head_ticket_gate->id = %d\n", events->head_ticket_gate->id);
	//else
	//	printf("La coda del cristo è nulla\n");

	if (state->queue_count > 0 && events->head_ticket_gate != NULL)
	{
		struct queue_node *job = (struct queue_node *)malloc(sizeof(struct queue_node));
		if (!job)
		{
			printf("Error in malloc in user departure ticket gate!\n");
			exit(-1);
		}

		job = events->head_ticket_gate;
		state->server_occupation[server_offset] = 1;
		events->completionTimes_ticket_gate[server_offset] = get_ticket_gate_departure(time->current);
		time->last[4] = time->current;
		state->queue_count--;
		state->server_count++;
		events->head_ticket_gate = events->head_ticket_gate->next;
		job->prev = NULL;
		job->next = NULL;
		free(job);
	}
	else
	{
		state->server_occupation[server_offset] = 0;
		events->completionTimes_ticket_gate[server_offset] = (double)INFINITY;
	}
	state->population = state->queue_count + state->server_count;

	printf("Evento di departure in ticket gate!\n");
	printf("state->queue_count = %d\n", state->queue_count);
	printf("state->server_count = %d\n", state->server_count);
}
