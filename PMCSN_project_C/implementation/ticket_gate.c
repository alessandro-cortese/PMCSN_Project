#include "./data_structures/event_list.h"
#include "./data_structures/states.h"
#include "./data_structures/area.h"
#include "./data_structures/time.h"
#include "./data_structures/loss.h"
#include "./data_structures/utils.h"
#include "./headers/ticket_machine.h"
#include "./headers/ticket_office.h"
#include "./headers/customer_support.h"
#include "./headers/security_check.h"
#include "./headers/ticket_gate.h"
#include "./headers/constants.h"
#include "./headers/rngs.h"
#include "./headers/utility_functions.h"
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

	int idle_offset = -1;
	for (int i = 0; i < NUMBER_OF_TICKET_GATE_SERVERS; i++)
	{
		if (state->server_occupation[i] == 0)
		{
			idle_offset = i;
			break;
		}
	}

	if (idle_offset >= 0 && events->head_ticket_gate != NULL)
	{
		loss->index_user += 1;
		state->population += 1;
		// Set idle server to busy server and update departure time
		state->server_occupation[idle_offset] = 1;
		events->completionTimes_ticket_gate[idle_offset] = get_ticket_gate_departure(rate);

		struct user *tail_job = (struct user *)malloc(sizeof(struct user));
		if (!tail_job)
		{
			printf("Error in malloc in user arrival in ticket gate!\n");
			exit(-1);
		}

		if (events->head_ticket_gate == events->tail_ticket_gate)
		{
			events->head_ticket_gate = NULL;
			free(events->tail_ticket_gate);
			events->tail_ticket_gate = NULL;
			events->user_arrival_to_ticket_gate.user_arrival_time = (double)INFINITY;
		}
		else
		{
			tail_job = events->head_ticket_gate;
			events->head_ticket_gate = tail_job->next;
			tail_job->next = NULL;
			tail_job->prev = NULL;
			free(tail_job);
			events->user_arrival_to_ticket_gate.user_arrival_time = time->current;
		}
	}
	
}
void user_departure_ticket_gate(struct event_list *events, struct time *time, struct states *state, struct loss *loss, int server_offset)
{
	state->population -= 1;
	events->completionTimes_security_check[server_offset] = (double)INFINITY;
	state->server_occupation[server_offset] = 0;
}
