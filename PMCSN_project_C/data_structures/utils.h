#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

struct next_abandon
{
    int user_Id;
    double abandonTime;
};

struct next_job
{
    int serverOffset;
    double completionTime;
};

struct infinite_horizon_stats
{
    // Struttura dati per salvare le statistiche dei centri in base al batch considerato
    double ***batch_statistics;
    // Prima struttura dati a supporto per il calcolo delle statistiche del batch
    double **sum;
    // Seconda struttura dati a supporto per il calcolo delle statistiche del batch
    double **mean;
    // these are variables for statistics in batch for infinite horizon
    int *sum_arrivals;
    double *batch_time;
};

#endif