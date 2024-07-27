package it.uniroma2.pmcsn.trainsimulator.engineer;

public class Constants{

    //Arrival rate of traveller to the station [req/sec]
    public static final double LAMBDA1 = 0.0625;            //[00 - 06]
    public static final double LAMBDA2 = 0.3864;            //[06 - 12]
    public static final double LAMBDA3 = 0.2727;            //[12 - 18]
    public static final double LAMBDA5 = 0.2784;            //[18 - 24]

    //Arrival to the differente queues [req/sec]
    public static final double ARRIVAL_TICKET_STATION = ;
    public static final double ARRIVAL_TICKET_OFFICE = ;
    public static final double ARRIVAL_CUSTOMER_SUPPORT = ;
    public static final double ARRIVAL_SECURITY_CONTROL = ;
    public static final double ARRIVAL_TICKET_GATE = ;

    //Probability of traveller that have already the ticket
    public static final double TRAVELLER_ALREADY_HAS_TICKET = ;

    
    // Probability of abandonment
    public static final double LEAVE_TICKET_STATION = 0.05;
    public static final double LEAVE_TICKET_OFFICE = 0.03;
    public static final double LEAVE_CUSTOMER_SUPPORT = 0.02;
    public static final double LEAVE_SECURITY_CONTROL = 0.01;   
    public static final double LEAVE_TICKET_GATE = 0.01;


    //Probability of having problems
    //This probability is relative to whether or not to access customer service stations
    public static final double PROBABILITY_OF_HAVING_PROBLEMS = 0.1;

    //Probability of security control
    public static final double PROBABILITY_OF_SECURITY_CONTROL = 0.05;

}