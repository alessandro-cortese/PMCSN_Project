double Exponential(double m);
bool is_system_empty(struct states *state, int *n);
struct next_abandon *get_min_abandon(struct abandon_node *head);
struct next_job *get_min_queue_time(struct event_list events, int num_servers, int *server_occupation, int index);
double get_smallest(double *values, int len);
double get_minimum_time(struct event_list events, struct states *state, int *n);
int get_total_busy_servers(int num_servers, int *server_list);
void routing_ticket_purchased(struct event_list *events, struct time *time, double rate);
void routing_security_check(struct event_list *events, struct time *time, double rate);
void routing_ticket_gate(struct event_list *events, struct time *time);
void feedback(struct event_list *events, struct time *time, double rate);
void verify(struct area *area, struct loss *loss, double time);