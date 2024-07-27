public enum Event{

    //TICKET_MACHINE events
    ARRIVAL_TICKET_MACHINE,
    TICKET_BUYED_FROM_TICKET_MACHINE,
    LEAVE_FROM_TICKET_MACHINE,

    //TICKET_OFFICE events
    ARRIVAL_TICKET_OFFICE,
    TICKET_BUYED_FROM_TICKET_OFFICE,
    LEAVE_FROM_TICKET_OFFICE,

    //CUSTOMER_SUPPORT events
    ARRIVAL_CUSTOMER_SUPPORT,
    LEAVE_FROM_CUSTOMER_SUPPORT_RESOLVED,       //Problem Solved
    LEAVE_FROM_CUSTOMER_SUPPORT,                //Problem NOT Soved

    //SECURITY_CONTROL events
    ARRIVAL_SECURITY_CONTROL,       
    LEAVE_FROM_SECURITY_CONTROL_PASS,           //Traveller can enter the gate
    LEAVE_FROM_SECURITY_CONTROL,                //Traveller can't enter the gate

    //TICKET_GATE events
    ARRIVAL_TICKET_GATE,
    LEAVE_FROM_TICKET_GATE_PASS,                //Traveller can access to the train
    LEAVE_FROM_TICKET_GATE                      //Traveller can't access to the train

}  