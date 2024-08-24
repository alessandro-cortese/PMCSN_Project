#ifndef UTILITY_FUNCTION_H
#define UTILITY_FUNCTION_H

double get_random(int index);
double Exponential(double m);
int lenOfQueue(struct queue_node *head);
bool is_system_empty(struct states *state, int *n);
struct next_abandon *get_min_abandon(struct queue_node *head);
struct next_job *get_min_queue_time(struct event_list events, int num_servers, int *server_occupation, int index);
double get_smallest(double *values, int len);
double get_minimum_time(struct event_list events, struct states *state, int *n);
int get_total_busy_servers(int num_servers, int *server_list);
void routing_ticket_purchased(struct event_list *events, struct time *time, double rate);
void routing_security_check(struct event_list *events, struct time *time, double rate);
void routing_ticket_gate(struct event_list *events, struct time *time);
void feedback(struct event_list *events, struct time *time, double rate);
void get_statistics_for_batch(int center_index, int *count, double ***batch_statistics, double **sum, double **mean, double diff, int *number_of_centers, struct area *area, struct time *time, struct loss *loss);
void verify(struct area *area, struct loss *loss, double time, struct time *t);
void consistency_check_population(struct event_list *events);
void enqueue_node(struct queue_node **head, struct queue_node **tail, struct queue_node *job);
void dequeue_node_free_node(struct queue_node **head);
struct queue_node *dequeue_node(struct queue_node **head);
void print_progress_bar(double part, double total, double oldPart);
#endif