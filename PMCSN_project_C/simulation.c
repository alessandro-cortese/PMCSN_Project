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
#include "./headers/rvms.h"
#include "./headers/utility_functions.h"
#include "./headers/ticket_purchased.h"
#include "./headers/validation.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

struct event_list events;
struct states state[5];
struct area areas[5];
struct time *t;
double last_event;
struct loss loss[5];
int *number_of_centers;
bool remaining[5] = {false};

struct infinite_horizon_stats inf_horizon_stats;

int count_user = 0;

struct infinite_horizon_stats *get_infinite_horizon_address()
{
	return &inf_horizon_stats;
}

struct area *get_area_address()
{
	return &areas[0];
}

struct states *get_first_state_address()
{
	// Return the address of the first element in the array
	return &state[0];
}
struct loss *get_first_loss()
{
	return &loss[0];
}

void initialize_number_of_centers()
{
	number_of_centers = (int *)malloc(sizeof(int) * QUEUE_NUMBER_CENTERS);
	if (number_of_centers == NULL)
	{
		printf("Error in first malloc\n");
		exit(-2);
	}
	// config of the network
	number_of_centers[0] = NUMBER_OF_TICKET_MACHINE_SERVER;
	number_of_centers[1] = NUMBER_OF_TICKET_OFFICE_SERVER;
	number_of_centers[2] = NUMBER_OF_CUSTOMER_SUPPORT_SERVER;
	number_of_centers[3] = NUMBER_OF_SECURITY_CHECK_SERVERS;
	number_of_centers[4] = NUMBER_OF_TICKET_GATE_SERVERS;
}

int *get_address_number_of_centers()
{
	return &number_of_centers[0];
}

void initialize_infinite_horizon_stats()
{
	/*
Struttura dati per salvare le statistiche dei centri in base al batch considerato
*/

	inf_horizon_stats.batch_statistics = (double ***)malloc(sizeof(double **) * QUEUE_NUMBER_CENTERS);
	if (inf_horizon_stats.batch_statistics == NULL)
	{
		printf("Error in first malloc in infinite horizon run!\n");
		exit(-1);
	}

	for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
	{
		inf_horizon_stats.batch_statistics[i] = (double **)malloc(sizeof(double *) * NUMBER_OF_STATISTICS);
		if (inf_horizon_stats.batch_statistics[i] == NULL)
		{
			printf("Error in second malloc in inifinite horizon simulation!\n");
			exit(-1);
		}

		for (int j = 0; j < NUMBER_OF_STATISTICS; j++)
		{
			inf_horizon_stats.batch_statistics[i][j] = (double *)malloc(sizeof(double) * K);
			if (inf_horizon_stats.batch_statistics[i][j] == NULL)
			{
				printf("Error in third malloc in inifinite server horizon!\n");
				exit(-1);
			}
		}
	}

	/*
	Prima struttura dati a supporto per il calcolo delle statistiche del batch
	*/

	inf_horizon_stats.sum = (double **)malloc(sizeof(double *) * QUEUE_NUMBER_CENTERS);
	if (inf_horizon_stats.sum == NULL)
	{
		printf("Error in malloc for sum in inifinite horizon run!\n");
		exit(-1);
	}
	for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
	{
		inf_horizon_stats.sum[i] = (double *)malloc(sizeof(double) * NUMBER_OF_STATISTICS);
		if (inf_horizon_stats.sum[i] == NULL)
		{
			printf("Error in malloc for sum[%d] in infinite horizon run!\n", i);
			exit(-1);
		}
	}

	/*
	Seconda struttura dati a supporto per il calcolo delle statistiche del batch
	*/

	inf_horizon_stats.mean = (double **)malloc(sizeof(double *) * QUEUE_NUMBER_CENTERS);
	if (inf_horizon_stats.mean == NULL)
	{
		printf("Error in malloc for mean in inifinite horizon run!\n");
		exit(-1);
	}
	for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
	{
		inf_horizon_stats.mean[i] = (double *)malloc(sizeof(double) * NUMBER_OF_STATISTICS);
		if (inf_horizon_stats.mean[i] == NULL)
		{
			printf("Error in malloc for mean[%d] in infinite horizon run!\n", i);
			exit(-1);
		}
	}

	inf_horizon_stats.sum_arrivals = (int *)malloc(sizeof(int) * QUEUE_NUMBER_CENTERS);
	if (!inf_horizon_stats.sum_arrivals)
	{
		printf("Error in malloc for sum_arrivals in inizialize infinite horizon run!\n");
		exit(-1);
	}

	inf_horizon_stats.batch_time = (double *)malloc(sizeof(double) * QUEUE_NUMBER_CENTERS);
	if (!inf_horizon_stats.batch_time)
	{
		printf("Error in mallc for batch_times inizialize inifite horizon run!\n");
		exit(-1);
	}

	for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
	{
		inf_horizon_stats.batch_time[i] = 0.0;
		inf_horizon_stats.sum_arrivals[i] = 0;
	}
}
void initializeEventList(int *n, double rate)
{
	events.user_who_has_purchased_ticket.user_arrival_time = get_user_arrival_to_ticket_purchased(START, rate);
	//  Init ticket machine
	events.user_arrival_to_ticket_machine.user_arrival_time = get_user_arrival_to_ticket_machine(START, rate);
	events.user_arrival_to_ticket_machine.is_user_arrival_active = true;
	// Init ticket office
	events.user_arrival_to_ticket_office.user_arrival_time = get_user_arrival_to_ticket_office(START, rate);
	events.user_arrival_to_ticket_office.is_user_arrival_active = true;
	// Init customer support
	events.user_arrival_to_customer_support.user_arrival_time = (double)INFINITY;
	events.user_arrival_to_customer_support.is_user_arrival_active = true;
	// Init security check
	events.user_arrival_to_security_check.user_arrival_time = (double)INFINITY;
	events.user_arrival_to_security_check.is_user_arrival_active = true;
	// Init ticket gate
	events.user_arrival_to_ticket_gate.user_arrival_time = (double)INFINITY;
	events.user_arrival_to_ticket_gate.is_user_arrival_active = true;

	events.head_ticket_machine = NULL;
	events.tail_ticket_machine = NULL;
	events.head_ticket_office = NULL;
	events.tail_ticket_office = NULL;

	events.completionTimes_ticket_machine = (double *)malloc(sizeof(double) * n[0]);
	events.completionTimes_ticket_office = (double *)malloc(sizeof(double) * n[1]);
	events.completionTimes_customer_support = (double *)malloc(sizeof(double) * n[2]);
	events.completionTimes_security_check = (double *)malloc(sizeof(double) * n[3]);
	events.completionTimes_ticket_gate = (double *)malloc(sizeof(double) * n[4]);

	for (int i = 0; i < n[0]; i++)
	{
		events.completionTimes_ticket_machine[i] = (double)INFINITY;
	}
	for (int i = 0; i < n[1]; i++)
	{
		events.completionTimes_ticket_office[i] = (double)INFINITY;
	}
	for (int i = 0; i < n[2]; i++)
	{
		events.completionTimes_customer_support[i] = (double)INFINITY;
	}
	for (int i = 0; i < n[3]; i++)
	{
		events.completionTimes_security_check[i] = (double)INFINITY;
	}
	for (int i = 0; i < n[4]; i++)
	{
		events.completionTimes_ticket_gate[i] = (double)INFINITY;
	}
}

void initializeTime()
{
	t = (struct time *)malloc(sizeof(struct time));
	t->current = 0.0;
	t->next = 0.0;
	for (int i = 0; i < 5; i++)
	{
		t->last[i] = 0.0;
	}
}
void initializeArea()
{

	for (int i = 0; i < 5; i++)
	{
		areas[i].node = 0.0;
		areas[i].queue = 0.0;
		areas[i].service = 0.0;
	}
}

void initializeStateVariables(int *m)
{
	for (int j = 0; j < QUEUE_NUMBER_CENTERS; j++)
	{
		state[j].population = 0;
		state[j].queue_count = 0;
		state[j].server_count = 0;
		state[j].server_occupation = (int *)malloc(sizeof(int) * m[j]);
		if (!state[j].server_occupation)
		{
			printf("Error in malloc in initializeStateVaribles!\n");
			exit(-1);
		}

		for (int i = 0; i < m[j]; i++)
		{
			state[j].server_occupation[i] = 0; // 0 = IDLE, 1 = BUSY
		}
	}
}

void initializeArrivalLoss()
{
	for (int i = 0; i < 5; i++)
	{
		loss[i].index_user = 0;
		loss[i].loss_user = 0;
	}
}

void calcultate_area_struct(int *n)
{
	int ticket_machine_busy_servers = get_total_busy_servers(n[0], state[0].server_occupation);
	areas[0].service += (t->next - t->current) * ticket_machine_busy_servers;
	areas[0].queue += (t->next - t->current) * (state[0].queue_count);
	areas[0].node += (t->next - t->current) * (state[0].queue_count + ticket_machine_busy_servers);

	int ticket_office_busy_servers = get_total_busy_servers(n[1], state[1].server_occupation);
	areas[1].service += (t->next - t->current) * ticket_office_busy_servers;
	areas[1].queue += (t->next - t->current) * (state[1].queue_count);
	areas[1].node += (t->next - t->current) * (state[1].queue_count + ticket_office_busy_servers);

	int customer_support_busy_servers = get_total_busy_servers(n[2], state[2].server_occupation);
	areas[2].service += (t->next - t->current) * customer_support_busy_servers;
	areas[2].queue += (t->next - t->current) * (state[2].queue_count);
	areas[2].node += (t->next - t->current) * (state[2].queue_count + customer_support_busy_servers);

	int security_check_busy_servers = get_total_busy_servers(n[3], state[3].server_occupation);
	areas[3].service += (t->next - t->current) * security_check_busy_servers;
	areas[3].queue += (t->next - t->current) * (state[3].queue_count);
	areas[3].node += (t->next - t->current) * (state[3].queue_count + security_check_busy_servers);

	int ticket_gate_busy_servers = get_total_busy_servers(n[4], state[4].server_occupation);
	areas[4].service += (t->next - t->current) * ticket_gate_busy_servers;
	areas[4].queue += (t->next - t->current) * (state[4].queue_count);
	areas[4].node += (t->next - t->current) * (state[4].queue_count + ticket_gate_busy_servers);
}

/*
Server per la simulazione a orizzonte finito, da fare dopo.
*/

// void write_rho_on_csv(int time_slot, struct area *area, double current_time)
// {
// 	FILE *csv_file;
// 	char *filename;
// 	filename = (char *)malloc(sizeof(char) * 100);
// 	if (!filename)
// 	{
// 		printf("Error in malloc in write rho on csv!\n");
// 		exit(-1);
// 	}
// 	const char *centers[] = {
// 		"ticket_machine",
// 		"ticket_office",
// 		"customer_support",
// 		"security_check",
// 		"ticket_gate"};

// 	snprintf(filename, 100, "./results/finite/rho/finite_horizion_simulation_time_slot_%d.csv", time_slot);
// 	csv_file = fopen(filename, "w+");
// 	if (csv_file == NULL)
// 	{
// 		printf("Error in fopen\n");
// 		exit(-1);
// 	}
// 	for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
// 	{
// 		double rho = 0.0;
// 		rho = area[i].service / (current_time * NUMBER_OF_TICKET_MACHINE_SERVER);
// 		fprintf(csv_file, "%2.6f\n", rho);
// 	}
// 	fclose(csv_file);
// }

void finite_horizon_run(double rate, double stop_value, int repetition_index, int time_slot)
{
	initializeTime();
	initializeArea();
	initialize_number_of_centers();
	initializeStateVariables(number_of_centers);
	initializeArrivalLoss();
	initializeEventList(number_of_centers, rate);

	// printf("\n\nRUN NUMBER %d OF FINITE HORIZON SIMULATION!\n", repetition_index);

	while (events.user_arrival_to_ticket_machine.is_user_arrival_active || events.user_arrival_to_ticket_office.is_user_arrival_active ||
		   events.user_who_has_purchased_ticket.is_user_arrival_active || !is_system_empty(state, number_of_centers))
	{
		t->next = get_minimum_time(events, state, number_of_centers);

		// printf("----------\n");
		// printf("t->current:%f\n", t->current);
		// printf("t->next: %f\n\n\n", t->next);
		calcultate_area_struct(number_of_centers);
		// printf("----------\n");

		if (t->next == t->current)
		{
			printf("Current time is equal to next time...\n");
			exit(-1);
		}
		// printf("----------\n");
		// printf("t->current:%f\n", t->current);
		// printf("t->next: %f\n\n\n", t->next);
		// printf("----------\n");

		struct next_abandon *next_ticket_machine_abandon = get_min_abandon(events.head_ticket_machine);
		struct next_abandon *next_ticket_office_abandon = get_min_abandon(events.head_ticket_office);
		struct next_abandon *next_customer_support_abandon = get_min_abandon(events.head_customer_support);

		struct next_job *next_job_ticket_machine = get_min_queue_time(events, number_of_centers[0], state[0].server_occupation, 1);
		struct next_job *next_job_ticket_office = get_min_queue_time(events, number_of_centers[1], state[1].server_occupation, 2);
		struct next_job *next_job_customer_support = get_min_queue_time(events, number_of_centers[2], state[2].server_occupation, 3);
		struct next_job *next_job_security_check = get_min_queue_time(events, number_of_centers[3], state[3].server_occupation, 4);
		struct next_job *next_job_ticket_gate = get_min_queue_time(events, number_of_centers[4], state[4].server_occupation, 5);

		double min_job_completion_ticket_machine = next_job_ticket_machine->completionTime;
		double min_job_completion_ticket_office = next_job_ticket_office->completionTime;
		double min_job_completion_customer_support = next_job_customer_support->completionTime;
		double min_job_completion_security_check = next_job_security_check->completionTime;
		double min_job_completion_ticket_gate = next_job_ticket_gate->completionTime;

		t->current = t->next;

		if (t->current == events.user_who_has_purchased_ticket.user_arrival_time)
		{
			// printf("*** Evento append_user_arrival_ticket_purchased ***\n");
			append_user_arrival_ticket_purchased(&events, t, rate, stop_value, false);
		}
		else if (t->current == events.user_arrival_to_ticket_machine.user_arrival_time)
		{
			// printf("*** Evento user_arrivals_ticket_machine ***\n");
			user_arrivals_ticket_machine(&events, t, &state[0], &loss[0], rate, stop_value);
		}
		else if (t->current == min_job_completion_ticket_machine)
		{
			// printf("*** Evento user_departure_ticket_machine ***\n");
			user_departure_ticket_machine(&events, t, &state[0], &loss[0], next_job_ticket_machine->serverOffset, rate, false);
		}
		else if (t->current == next_ticket_machine_abandon->abandonTime)
		{
			// printf("*** Evento abandon_ticket_machine ***\n");
			abandon_ticket_machine(&events, &state[0], &loss[0], next_ticket_machine_abandon->user_Id);
		}
		else if (t->current == events.user_arrival_to_ticket_office.user_arrival_time)
		{
			// printf("*** Evento user_arrivals_ticket_office ***\n");
			user_arrivals_ticket_office(&events, t, &state[1], &loss[1], rate, stop_value);
		}
		else if (t->current == min_job_completion_ticket_office)
		{
			// printf("*** Evento user_departure_ticket_office ***\n");
			user_departure_ticket_office(&events, t, &state[1], &loss[1], next_job_ticket_office->serverOffset, rate, false);
		}
		else if (t->current == next_ticket_office_abandon->abandonTime)
		{
			// printf("*** Evento abandon_ticket_office ***\n");
			abandon_ticket_office(&events, &state[1], &loss[1], next_ticket_office_abandon->user_Id);
		}
		else if (t->current == min_job_completion_customer_support)
		{
			// printf("*** Evento user_departure_customer_support ***\n");
			user_departure_customer_support(&events, t, &state[2], &loss[2], next_job_customer_support->serverOffset, rate, false);
		}
		else if (t->current == next_customer_support_abandon->abandonTime)
		{
			// printf("*** Evento abandon_customer_support ***\n");
			abandon_customer_support(&events, &state[2], &loss[2], next_customer_support_abandon->user_Id);
		}
		else if (t->current == min_job_completion_security_check)
		{
			// printf("*** Evento user_departure_security_check! ***\n");
			user_departure_security_check(&events, t, &state[3], &loss[3], next_job_security_check->serverOffset, false);
		}
		else if (t->current == min_job_completion_ticket_gate)
		{
			// printf("*** Evento user_departure_ticket_gate ***\n");
			user_departure_ticket_gate(&events, t, &state[4], &loss[4], next_job_ticket_gate->serverOffset);
		}

		free(next_ticket_machine_abandon);
		free(next_ticket_office_abandon);
		free(next_customer_support_abandon);

		free(next_job_ticket_machine);
		free(next_job_ticket_office);
		free(next_job_customer_support);
		free(next_job_security_check);
		free(next_job_ticket_gate);

		if (t->next == (double)INFINITY)
		{
			printf("Something went wrong with simulation...\n");
			exit(-1);
		}

		consistency_check_population(&events);
		last_event = t->current;
	}

	verify(areas, loss, last_event, t);
	// printf("\n\nEND FINITE HORIZON RUN REPETITION #%d\n", repetition_index);
}

void finite_horizon_simulation(double rate, double stop, int num_repetition, int time_slot)
{
	printf("\n\n FINITE HORIZON SIMULATION\nINTERARRIVAL = %f | STOP TIME = %f | REPETITION = %d\n", rate, stop, num_repetition);

	/*
	Funzioni utilizzate per inizializzare le strutture dati per la simulazione ad orizzonte finito.
	*/

	long seed[NUM_REPETITION + 1];
	seed[0] = SEED;

	for (int repetition = 0; repetition < num_repetition; repetition++)
	{
		PlantSeeds(seed[repetition]);
		finite_horizon_run(rate, stop, repetition, time_slot);
		SelectStream(300);
		GetSeed(&seed[repetition + 1]);
		print_progress_bar("Simulation progress", repetition + 1, NUM_REPETITION, repetition);
		sleep(1);
	}

	// write_rho_on_csv(time_slot, areas, t->current);
	print_progress_bar("Simulation progress", num_repetition, NUM_REPETITION, num_repetition - 1);
	printf("\n\nEND FINITE HORIZON SIMULATION\n");
}

void infinite_horizon_run(double rate)
{
	initializeTime();
	initializeArea();
	initialize_number_of_centers();
	initializeStateVariables(number_of_centers);
	initializeArrivalLoss();
	initializeEventList(number_of_centers, rate);
	initialize_infinite_horizon_stats();

	// int remaining = 0;
	int batch_counter_ticket_machine = 0; // Conta i batch completati
	int batch_counter_ticket_office = 0;
	int batch_counter_customer_support = 0;
	int batch_counter_security_check = 0;
	int batch_counter_ticket_gate = 0;
	printf("\n\nINFINITE HORIZON SIMULATION!\n");
	while (/*remaining[0] == false && remaining[1] == false && remaining[2] == false &&
		   remaining[3] == false && remaining[4] == false*/
		   true)
	{

		// remaining = 0;
		// for (int i = 0; i < 5; i++)
		// {
		// 	if (inf_horizon_stats.sum_arrivals[i] < K * B)
		// 		remaining++;
		// }
		// if (remaining == 0)
		// 	break;

		// for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
		// {
		// 	if (inf_horizon_stats.sum_arrivals[i] == K * B)
		// 		remaining[i] = true;
		// }
		bool all_centers_completed = true;

		for (int i = 0; i < 5; i++)
		{
			if (inf_horizon_stats.sum_arrivals[i] < K * B)
			{
				remaining[i] = false;
				all_centers_completed = false;
			}
			else
			{
				remaining[i] = true;
			}
		}

		if (all_centers_completed)
			break;

		t->next = get_minimum_time(events, state, number_of_centers);

		// printf("----------\n");
		// printf("t->current:%f\n", t->current);
		// printf("t->next: %f\n\n\n", t->next);
		calcultate_area_struct(number_of_centers);
		// printf("----------\n");

		if (t->next == t->current)
		{
			printf("Current time is equal to next time...\n");
			exit(-1);
		}
		// printf("----------\n");
		// printf("t->current:%f\n", t->current);
		// printf("t->next: %f\n\n\n", t->next);
		// printf("----------\n");

		struct next_abandon *next_ticket_machine_abandon = get_min_abandon(events.head_ticket_machine);
		struct next_abandon *next_ticket_office_abandon = get_min_abandon(events.head_ticket_office);
		struct next_abandon *next_customer_support_abandon = get_min_abandon(events.head_customer_support);

		struct next_job *next_job_ticket_machine = get_min_queue_time(events, number_of_centers[0], state[0].server_occupation, 1);
		struct next_job *next_job_ticket_office = get_min_queue_time(events, number_of_centers[1], state[1].server_occupation, 2);
		struct next_job *next_job_customer_support = get_min_queue_time(events, number_of_centers[2], state[2].server_occupation, 3);
		struct next_job *next_job_security_check = get_min_queue_time(events, number_of_centers[3], state[3].server_occupation, 4);
		struct next_job *next_job_ticket_gate = get_min_queue_time(events, number_of_centers[4], state[4].server_occupation, 5);

		double min_job_completion_ticket_machine = next_job_ticket_machine->completionTime;
		double min_job_completion_ticket_office = next_job_ticket_office->completionTime;
		double min_job_completion_customer_support = next_job_customer_support->completionTime;
		double min_job_completion_security_check = next_job_security_check->completionTime;
		double min_job_completion_ticket_gate = next_job_ticket_gate->completionTime;

		t->current = t->next;

		if (t->current == events.user_who_has_purchased_ticket.user_arrival_time)
		{
			// printf("*** Evento append_user_arrival_ticket_purchased ***\n");
			append_user_arrival_ticket_purchased(&events, t, rate, (double)INFINITY, true);
		}
		else if (t->current == events.user_arrival_to_ticket_machine.user_arrival_time)
		{
			// printf("*** Evento user_arrivals_ticket_machine ***\n");
			user_arrivals_ticket_machine(&events, t, &state[0], &loss[0], rate, (double)INFINITY);
			if (inf_horizon_stats.sum_arrivals[0] < B * K)
			{
				inf_horizon_stats.sum_arrivals[0] += 1;
				if (inf_horizon_stats.sum_arrivals[0] % B == 0)
				{
					get_statistics_for_batch(0, &inf_horizon_stats, number_of_centers, areas, t, loss[0]);
					inf_horizon_stats.batch_time[0] = t->current;
				}
			}
		}
		else if (t->current == min_job_completion_ticket_machine)
		{
			// printf("*** Evento user_departure_ticket_machine ***\n");
			user_departure_ticket_machine(&events, t, &state[0], &loss[0], next_job_ticket_machine->serverOffset, rate, true);
		}
		else if (t->current == next_ticket_machine_abandon->abandonTime)
		{
			// printf("*** Evento abandon_ticket_machine ***\n");
			abandon_ticket_machine(&events, &state[0], &loss[0], next_ticket_machine_abandon->user_Id);
		}
		else if (t->current == events.user_arrival_to_ticket_office.user_arrival_time)
		{
			// printf("*** Evento user_arrivals_ticket_office ***\n");
			user_arrivals_ticket_office(&events, t, &state[1], &loss[1], rate, (double)INFINITY);
			if (inf_horizon_stats.sum_arrivals[1] < B * K)
			{
				inf_horizon_stats.sum_arrivals[1] += 1;
				if (inf_horizon_stats.sum_arrivals[1] % B == 0)
				{
					get_statistics_for_batch(1, &inf_horizon_stats, number_of_centers, areas, t, loss[1]);
					inf_horizon_stats.batch_time[1] = t->current;
				}
			}
		}
		else if (t->current == min_job_completion_ticket_office)
		{
			// printf("*** Evento user_departure_ticket_office ***\n");
			user_departure_ticket_office(&events, t, &state[1], &loss[1], next_job_ticket_office->serverOffset, rate, true);
		}
		else if (t->current == next_ticket_office_abandon->abandonTime)
		{
			// printf("*** Evento abandon_ticket_office ***\n");
			abandon_ticket_office(&events, &state[1], &loss[1], next_ticket_office_abandon->user_Id);
		}
		else if (t->current == min_job_completion_customer_support)
		{
			// printf("*** Evento user_departure_customer_support ***\n");
			user_departure_customer_support(&events, t, &state[2], &loss[2], next_job_customer_support->serverOffset, rate, true);
		}
		else if (t->current == next_customer_support_abandon->abandonTime)
		{
			// printf("*** Evento abandon_customer_support ***\n");
			abandon_customer_support(&events, &state[2], &loss[2], next_customer_support_abandon->user_Id);
		}
		else if (t->current == min_job_completion_security_check)
		{
			// printf("*** Evento user_departure_security_check! ***\n");
			user_departure_security_check(&events, t, &state[3], &loss[3], next_job_security_check->serverOffset, true);
		}
		else if (t->current == min_job_completion_ticket_gate)
		{
			// printf("*** Evento user_departure_ticket_gate ***\n");
			user_departure_ticket_gate(&events, t, &state[4], &loss[4], next_job_ticket_gate->serverOffset);
		}

		free(next_ticket_machine_abandon);
		free(next_ticket_office_abandon);
		free(next_customer_support_abandon);

		free(next_job_ticket_machine);
		free(next_job_ticket_office);
		free(next_job_customer_support);
		free(next_job_security_check);
		free(next_job_ticket_gate);

		if (t->next == (double)INFINITY)
		{
			printf("Something went wrong with simulation...\n");
			exit(-1);
		}

		consistency_check_population(&events);
		last_event = t->current;

		// int batch_completed_ticket_machine = inf_horizon_stats.sum_arrivals[0] / B;
		// if (batch_counter_ticket_machine < batch_completed_ticket_machine)
		// {
		// 	batch_counter_ticket_machine = batch_completed_ticket_machine;

		// 	print_progress_bar("ticket machine", batch_counter_ticket_machine, K, batch_counter_ticket_machine + 1);
		// }

		// int batch_completed_ticket_office = inf_horizon_stats.sum_arrivals[1] / B;
		// if (batch_counter_ticket_office < batch_completed_ticket_office)
		// {
		// 	batch_counter_ticket_office = batch_completed_ticket_office;

		// 	print_progress_bar("ticket office", batch_counter_ticket_office, K, batch_counter_ticket_office + 1);
		// }

		// int batch_completed_customer_support = inf_horizon_stats.sum_arrivals[2] / B;
		// if (batch_counter_customer_support < batch_completed_customer_support)
		// {
		// 	batch_counter_customer_support = batch_completed_customer_support;

		// 	print_progress_bar("customer support", batch_counter_customer_support, K, batch_counter_customer_support + 1);
		// }

		int batch_completed_security_check = inf_horizon_stats.sum_arrivals[3] / B;
		if (batch_counter_security_check < batch_completed_security_check)
		{
			batch_counter_security_check = batch_completed_security_check;
			print_progress_bar("security check", batch_counter_security_check, K, batch_counter_security_check + 1);
		}

		// int batch_completed_ticket_gate = inf_horizon_stats.sum_arrivals[4] / B;
		// if (batch_counter_ticket_gate < batch_completed_ticket_gate)
		// {
		// 	batch_counter_ticket_gate = batch_completed_ticket_gate;

		// 	print_progress_bar("ticket gate", batch_counter_ticket_gate, K, batch_counter_ticket_gate + 1);
		// }

		// if (batch_counter == 128)
		// {
		// 	printf("all centers completed = %d\n", all_centers_completed);
		// 	for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
		// 	{
		// 		printf("remaining[%d] is %d\n", i, remaining[i]);
		// 		printf("inf_horizon_stats.sum_arrivals[%d] is %d\n", i, inf_horizon_stats.sum_arrivals[i]);
		// 	}
		// 	// exit(-1);
		// }
		// print_progress_bar(i, K, i + 1);
		//  sleep(1);
	}

	double **w = (double **)malloc(sizeof(double *) * QUEUE_NUMBER_CENTERS);
	if (w == NULL)
	{
		printf("Error in malloc for w!\n");
		exit(-1);
	}

	for (int j = 0; j < QUEUE_NUMBER_CENTERS; j++)
	{
		w[j] = (double *)malloc(sizeof(double) * NUMBER_OF_STATISTICS);
		if (w[j] == NULL)
		{
			printf("Error in malloc for w[%d]!\n", j);
			exit(-1);
		}
	}

	for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
	{
		for (int j = 0; j < NUMBER_OF_STATISTICS; j++)
		{
			w[i][j] = 0.0;
		}
	}

	int n;
	double u, t, stdv;
	FILE *fp;
	FILE **statistics_files = create_statistic_files();
	char *stat_value;

	puts("");
	for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
	{
		for (int j = 0; j < NUMBER_OF_STATISTICS; j++)
		{
			n = inf_horizon_stats.sum_arrivals[i] / B;
			stdv = sqrt(inf_horizon_stats.sum[i][j] / n);
			u = 1.0 - 0.5 * (1.0 - LEVEL_OF_CONFIDENCE); /* interval parameter  */
			t = idfStudent(n - 1, u);					 /* critical value of t */
			w[i][j] = t * stdv / sqrt(n - 1);			 /* interval half width */

			printf("INTERVALLO-INFINITO-statistica-%d-centro-%d ------ %10.6f +/- %6.6f\n", j, i, inf_horizon_stats.mean[i][j], w[i][j]);
			fflush(stdout);
		}
	}

	// PRINT
	for (int center = 0; center < QUEUE_NUMBER_CENTERS; center++)
	{
		fp = statistics_files[center];

		for (int stat = 0; stat < NUMBER_OF_STATISTICS; stat++)
		{
			for (int batch = 0; batch < K; batch++)
			{

				stat_value = (char *)malloc(sizeof(char) * 30);
				if (stat_value == NULL)
				{
					printf("Error in malloc for stat_value!\n");
					exit(-1);
				}

				sprintf(stat_value, "%f;", inf_horizon_stats.batch_statistics[center][stat][batch]);
				fputs(stat_value, fp);
				free(stat_value);

				fputs("\n", fp);
			}
			fputs("\n", fp);
		}
	}
}

void infinite_horizon_simulation(double rate)
{
	printf("\n\n INFINITE HORIZON SIMULATION\nINTERARRIVAL = %f | #BATCH = %d\n", rate, B);

	/*
	Funzioni utilizzate per inizializzare le strutture dati per la simulazione ad orizzonte infinito.
	*/

	long seed[NUM_REPETITION + 1];
	seed[0] = SEED;
	infinite_horizon_run(rate);
}

int main(int argc, char **argv)
{

	if (argc < 3)
	{
		printf("Usage: ./simulation <finite/infinite> <time_slot>\n");
		exit(-1);
	}

	int interval = atoi(argv[2]);
	// Verifica se il primo argomento è "finite" o "infinite"
	int is_finite = 0; // 0 per infinite, 1 per finite
	if (strcmp(argv[1], "finite") == 0)
	{
		is_finite = 1;
	}
	else if (strcmp(argv[1], "infinite") != 0)
	{
		printf("Invalid mode. Use 'finite' or 'infinite'.\n");
		exit(-1);
	}

	if (is_finite)
	{
		finite_horizon_simulation(ARRIVAL_1, STOP_INTERVAL_1, NUM_REPETITION, 1);
		finite_horizon_simulation(ARRIVAL_2, STOP_INTERVAL_2, NUM_REPETITION, 2);
		finite_horizon_simulation(ARRIVAL_3, STOP_INTERVAL_3, NUM_REPETITION, 3);
	}
	else if (!is_finite)
	{
		if (interval == 1)
			infinite_horizon_simulation(ARRIVAL_1);
		else if (interval == 2)
			infinite_horizon_simulation(ARRIVAL_2);
		else if (interval == 3)
			infinite_horizon_simulation(ARRIVAL_3);
	}
	return 0;
}