package it.uniroma2.pmcsn.trainsimulator.centers;

import java.util.ArrayList;
import java.util.List;

import static it.uniroma2.pmcsn.trainsimulator.engineer.Constants.QUEUE_NUMBER_CENTERS;
import it.uniroma2.pmcsn.trainsimulator.model.MsqSum;
import it.uniroma2.pmcsn.trainsimulator.model.MsqT;
import it.uniroma2.pmcsn.trainsimulator.engineer.EventListEntry;


//Class to do the similuation for the basic model of the system.

public class TrainSimulatorController {

    long number = 0;                                /*number in the node*/
    public static long counter;
    int e;                                          /*next event index*/
    int s;                                          /*server index*/
    private long jobServed = 0;                     /*contatore jobs processati*/
    private double area = 0.0;                      /*time integrated number in the node*/

    private final List<MsqSum> sum = new ArrayList<>(QUEUE_NUMBER_CENTERS + 1);
    
    private final MsqT time = new MsqT();
    private final List<EventListEntry> eventListSistema = new ArrayList<>(QUEUE_NUMBER_CENTERS);

    private List<Simulation> simulationList = new ArrayList<>(QUEUE_NUMBER_CENTERS);

    public TrainSimulatorController() throws Exception{
        
        

    }
}
