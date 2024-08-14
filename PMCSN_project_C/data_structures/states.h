#ifndef STATE_H
#define STATE_H

struct states
{
    int population;
    int server_count;
    int queue_count;
    int *server_occupation;
};

#endif