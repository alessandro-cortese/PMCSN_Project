package it.uniroma2.pmcsn.trainsimulator.engineer;

public class Constants{

    //Arrival rate of traveller to the station [req/sec]
    public static final double LAMBDA1 = 0.129;                                                 //[05 - 08]
    public static final double LAMBDA2 = 0.6309;                                                //[08 - 19]
    public static final double LAMBDA3 = 0.211;                                                 //[19 - 24]

    //Probability
    public static final double PROBABILITY_OF_TICKET_PURCHASED_ONLINE = 0.6321;
    public static final double PROBABILITY_OF_TICKET_PURCHASED_IN_STATION = 0.3679;
    public static final double PROBABILITY_OF_PURCHASED_TICKET_FROM_TICKET_STATION = 0.6;
    public static final double PROBABILITY_OF_PURCHASED_TICKET_FROM_TICKET_OFFICE = 0.4;
    public static final double PROBABILITY_OF_CUSTOMER_SUPPOR = 0.15;
    public static final double PROBABILITY_OF_NOT_CUSTOMER_SUPPORT = 0.75;
    public static final double PROBABILITY_OF_SECURITY_CONTROL = 0.10;
    public static final double PROBABILITY_OF_NOT_SECURITY_CONTROL = 0.9;
    public static final double PROBABILITY_OF_CHANGE_TICKET = 0.07;
    public static final double PROBABILITY_OF_NOT_CHANGE_TICKET = 0.93;
    // Probability of abandonment
    
    public static final double PROBABILITY_OF_LEAVE_TICKET_STATION = 0.05;
    public static final double PROBABILITY_OF_LEAVE_TICKET_OFFICE = 0.03;
    public static final double PROBABILITY_OF_LEAVE_CUSTOMER_SUPPORT = 0.015;
    public static final double PROBABILITY_OF_LEAVE_SECURITY_CONTROL = 0.01;   
    public static final double PROBABILITY_OF_LEAVE_TICKET_GATE = 0.01;

    /**Arrival to the differente queues [req/sec] depends on:
    *- the value of time slot input rate;
    *- the probability of leave the queues.
    *The arrival are:
    *- ARRIVAL_TICKET_STATION
    *- ARRIVAL_TICKET_OFFICE
    *- ARRIVAL_CUSTOMER_SUPPORT
    *- ARRIVAL_SECURITY_CONTROL
    *- ARRIVAL_TICKET_GATE
    */

    //Service Rates [sec]
    public static final double SERVICE_RATE_TICKET_STATION = 90;
    public static final double SERVICE_RATE_TICKET_OFFICE_OPERATOR = 240;
    public static final double SERVICE_RATE_CUSTOMER_SUPPORT_OPERATOR = 450;
    public static final double SERVICE_RATE_SECURITY_CONTROL_OPERATOR = 210;
    public static final double SERVICE_RATE_TICKET_GATE_OPERATOR = 45;

    //Total number of queues
    public static final double QUEUE_NUMBER_CENTERS = 4;
    
}