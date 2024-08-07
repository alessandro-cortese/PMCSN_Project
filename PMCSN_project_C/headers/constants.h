#define START 0.0
#define STOP 10000.0
#define SEED 123456789

// Dimension of array of time
#define DIM 15

// Number of servers of ticket machine service node
#define NUMBER_OF_TICKET_MACHINE_SERVER 10

// Number of servers of ticket office service node
#define NUMBER_OF_TICKET_OFFICE_SERVER 5

// Number of servers of customer support service node
#define NUMBER_OF_CUSTOMER_SUPPORT_SERVER 5

// Number of servers of security check service node
#define NUMBER_OF_SECURITY_CHECK_SERVERS 3

// Number of servers of ticket gate service node
#define NUMBER_OF_TICKET_GATE_SERVERS 10

// Arrival rate of traveller to the station [req/sec]
#define LAMBDA1 0.129  // [05 - 08]
#define LAMBDA2 0.6309 // [08 - 19]
#define LAMBDA3 0.211  // [19 - 24]

// Probability
#define P_OF_TICKET_PURCHASED_ONLINE 0.6321
#define P_TICKET_NOT_PURCHASED 0.3679
#define P_TICKET_PURCHASED_FROM_TICKET_STATION 0.6
#define P_TICKET_PURCHASED_FROM_TICKET_OFFICE 0.4
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
#define SR_TICKET_OFFICE_OPERATOR 240
#define SR_CUSTOMER_SUPPORT_OPERATOR 450
#define SR_SECURITY_CONTROL_OPERATOR 210
#define SR_TICKET_GATE_OPERATOR 45

// Total number of queues
#define QUEUE_NUMBER_CENTERS 5