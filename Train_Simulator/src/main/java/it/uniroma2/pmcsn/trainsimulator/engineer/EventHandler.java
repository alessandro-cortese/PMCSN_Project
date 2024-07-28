package it.uniroma2.pmcsn.trainsimulator.engineer;

import java.util.List;

public class EventHandler {
    
    private static EventHandler instance = null;

    private List<EventListEntry> eventsTicketsMachine;
    private List<EventListEntry> eventsTicketOffice;
    private List<EventListEntry> eventsCustomerSuppert;
    private List<EventListEntry> eventsSecucrityControl;
    private List<EventListEntry> eventsTicketGate;

    private EventHandler(){

    }

    public static EventHandler getInstance(){
        if(instance == null) {
            instance = new EventHandler();
        }
        return instance;
    }

}
