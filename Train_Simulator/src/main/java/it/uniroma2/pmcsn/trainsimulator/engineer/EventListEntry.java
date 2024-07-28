package it.uniroma2.pmcsn.trainsimulator.engineer;

import java.util.List;

//TO-DO: write the doc
 
public class EventListEntry {
    
    private double time;
    private int x;
    
    public EventListEntry(double time, int x){
        this.time = time;
        this.x=x;
    }

    public double getT() {
        return time;
    }

    public void setT(double t) {
        this.time = t;
    }

    public int getX() {
        return x;
    }

    public void setX(int x) {
        this.x = x;
    }
    
    public static int getNextEvent(List<EventListEntry> event, int channels){
        int e;
        int i = 0;

        while (event.get(i).x == 0)         /* find the index of the first 'active' */
            i++;                            /* element in the event list            */
        e = i;
        while (i < channels) {              /* now, check the others to find which  */
            i++;                            /* event type is most imminent          */
            if ((event.get(i).x == 1) && (event.get(i).time < event.get(e).time))
                e = i;
        }
        return e;
    }

}
