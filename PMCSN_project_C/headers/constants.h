#define START 0.0
#define STOP 14400.0
#define SEED 123456789

// Dimension of array of time
#define DIM 13

// Number of servers of ticket machine service node
#define NUMBER_OF_TICKET_MACHINE_SERVER 10

// Number of servers of ticket office service node
#define NUMBER_OF_TICKET_OFFICE_SERVER 5

// Number of servers of customer support service node
#define NUMBER_OF_CUSTOMER_SUPPORT_SERVER 5

// Number of servers of security check service node
#define NUMBER_OF_SECURITY_CHECK_SERVERS 3

// Number of servers of ticket gate service node
#define NUMBER_OF_TICKET_GATE_SERVERS 15

// Interarrival time [sec]
#define ARRIVAL_1 7.751937 // [05 - 08] -> STOP: 10800
#define ARRIVAL_2 1.585037 // [08 - 19] -> STOP: 39600
#define ARRIVAL_3 4.739336 // [19 - 24] -> STOP: 14400

// Probability
#define P_OF_TICKET_PURCHASED_ONLINE 0.6321
#define P_TICKET_PURCHASED_FROM_TICKET_STATION 0.22074
#define P_TICKET_PURCHASED_FROM_TICKET_OFFICE 0.14716
#define P_OF_CUSTOMER_SUPPORT 0.15
#define P_OF_NOT_CUSTOMER_SUPPORT 0.75
#define P_OF_SECURITY_CHECK 0.10
#define P_OF_NOT_SECURITY_CONTROL 0.9
#define P_OF_CHANGE_TICKET 0.07
#define P_OF_NOT_CHANGE_TICKET 0.93

// Probability of abandonment
#define P_LEAVE_TICKET_STATION 0.05
#define P_LEAVE_TICKET_OFFICE 0.03
#define P_LEAVE_CUSTOMER_SUPPORT 0.015
#define P_LEAVE_SECURITY_CONTROL 0.01
#define P_LEAVE_TICKET_GATE 0.01

// Service Rates [sec]
#define SR_TICKET_STATION 90
#define SR_TICKET_OFFICE_OPERATOR 180
#define SR_CUSTOMER_SUPPORT_OPERATOR 190
#define SR_SECURITY_CONTROL_OPERATOR 150
#define SR_TICKET_GATE_OPERATOR 45

// Total number of queues
#define QUEUE_NUMBER_CENTERS 5