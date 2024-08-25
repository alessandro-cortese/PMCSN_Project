#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
// Function prototypes
double get_user_arrival_to_ticket_purchased(double arrival, double rate);
struct states *get_first_state_address();
struct loss *get_first_loss();
struct infinite_horizon_stats *get_infinite_horizon_address();
struct area *get_area_address();
int *get_address_number_of_centers();

#endif