package it.uniroma2.pmcsn.trainsimulator.engineer;

public class Event{

    //TICKET_MACHINE events
    public static final int ARRIVAL_TICKET_MACHINE = 1;
    public static final int TICKET_BUYED_FROM_TICKET_MACHINE = 1;
    public static final int LEAVE_FROM_TICKET_MACHINE = 1;

    public static final int ALL_TICKET_MACHINE = ARRIVAL_TICKET_MACHINE + TICKET_BUYED_FROM_TICKET_MACHINE +  LEAVE_FROM_TICKET_MACHINE;

    //TICKET_OFFICE events
    public static final int ARRIVAL_TICKET_OFFICE = 1;
    public static final int TICKET_BUYED_FROM_TICKET_OFFICE = 1;
    public static final int LEAVE_FROM_TICKET_OFFICE = 1;

    public static final int ALL_TICKET_OFFICE_EVENTS = ARRIVAL_TICKET_OFFICE + TICKET_BUYED_FROM_TICKET_OFFICE + LEAVE_FROM_TICKET_OFFICE;

    //CUSTOMER_SUPPORT events
    public static final int ARRIVAL_CUSTOMER_SUPPORT = 1;
    public static final int LEAVE_FROM_CUSTOMER_SUPPORT_RESOLVED = 1;       //Problem Solved
    public static final int LEAVE_FROM_CUSTOMER_SUPPORT = 1;                //Problem NOT Soved

    public static final int ALL_CUSTOMER_SUPPORT_EVENT = ARRIVAL_CUSTOMER_SUPPORT + LEAVE_FROM_CUSTOMER_SUPPORT_RESOLVED + LEAVE_FROM_CUSTOMER_SUPPORT;

    //SECURITY_CONTROL events
    public static final int ARRIVAL_SECURITY_CONTROL = 1;       
    public static final int LEAVE_FROM_SECURITY_CONTROL_PASS = 1;           //Traveller can enter the gate
    public static final int LEAVE_FROM_SECURITY_CONTROL = 1;                //Traveller can't enter the gate

    public static final int ALL_SECURITY_CONTROL_EVENT = ARRIVAL_SECURITY_CONTROL + LEAVE_FROM_SECURITY_CONTROL_PASS + LEAVE_FROM_SECURITY_CONTROL; 

    //TICKET_GATE events
    public static final int ARRIVAL_TICKET_GATE = 1;
    public static final int LEAVE_FROM_TICKET_GATE_PASS = 1;                //Traveller can access to the train
    public static final int LEAVE_FROM_TICKET_GATE = 1;                      //Traveller can't access to the train

    public static final int ALL_TICKET_GATE_EVENTS = ARRIVAL_TICKET_GATE + LEAVE_FROM_TICKET_GATE_PASS + LEAVE_FROM_TICKET_GATE;

}  