#ifndef TIME_H		
#define TIME_H

struct time {
    double current;
    double next;
    double last[5]; //todo: why 5?
};

#endif