#ifndef UTILS_H		
#define UTILS_H

#include <stdbool.h>

struct next_abandon {
    int user_Id;
    double abandonTime;
};

struct next_completion {
    int serverOffset;
    double completionTime; 
};

#endif