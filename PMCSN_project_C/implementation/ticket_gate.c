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
	SelectStream(12);
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
	time->last[4] = time->current;

	if (idle_offset >= 0 && events->head_ticket_gate != NULL)
	{
		state->server_count++;
		dequeue_node(&events->head_ticket_gate);
		state->queue_count--;
		// Set idle server to busy server and update departure time
		state->server_occupation[idle_offset] = 1;
		events->completionTimes_ticket_gate[idle_offset] = get_ticket_gate_departure(time->current);
	}

	state->population = state->queue_count + state->server_count;
}
void user_departure_ticket_gate(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	state->server_count--;

	if (state->queue_count > 0 && events->head_ticket_gate != NULL)
	{

		state->server_occupation[server_offset] = 1;
		events->completionTimes_ticket_gate[server_offset] = get_ticket_gate_departure(time->current);
		state->queue_count--;
		state->server_count++;
		dequeue_node_free_node(&events->head_ticket_gate);
	}
	else
	{
		state->server_occupation[server_offset] = 0;
		events->completionTimes_ticket_gate[server_offset] = (double)INFINITY;
	}
	state->population = state->queue_count + state->server_count;
}
