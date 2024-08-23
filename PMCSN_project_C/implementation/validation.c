#include "../data_structures/event_list.h"
#include "../data_structures/states.h"
#include "../data_structures/area.h"
#include "../data_structures/time.h"
#include "../data_structures/loss.h"
#include "../data_structures/utils.h"
#include "../headers/ticket_machine.h"
#include "../headers/ticket_office.h"
#include "../headers/customer_support.h"
#include "../headers/security_check.h"
#include "../headers/ticket_gate.h"
#include "../headers/constants.h"
#include "../headers/rngs.h"
#include "../headers/utility_functions.h"
#include "../headers/ticket_purchased.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// void compute_interval(char *filename)
// {

//     long n = 0; /* counts data points */
//     double sum = 0.0;
//     double mean = 0.0;
//     double data;
//     double stdev;
//     double u, t, w;
//     double diff;

//     FILE *fp = fopen(filename, "r");
//     if (fp == NULL)
//         exit(-1);

//     double variate;
//     int ret;

//     while ((ret = fscanf(fp, "%lf", &variate)) != EOF)
//     {
//         if (ret != 1)
//             exit(-3);
//         n++;
//         diff = variate - mean;
//         printf("diff = %lf\n", diff);
//         fflush(stdout);
//         sum += diff * diff * (n - 1.0) / n;
//         printf("sum = %lf\n", sum);
//         fflush(stdout);
//         mean += diff / n;
//         printf("mean = %lf\n", mean);
//         fflush(stdout);
//         // printf("%lf\n", variate);
//     }

//     stdev = sqrt(sum / n);
//     printf("stdev = %lf\n", stdev);
//     fflush(stdout);

//     if (n > 1)
//     {
//         u = 1.0 - 0.5 * (1.0 - LEVEL_OF_CONFIDENCE); /* interval parameter  */
//         t = idfStudent(n - 1, u);                    /* critical value of t */
//         printf("Valore critico: %lf\n", t);
//         w = t * stdev / sqrt(n - 1); /* interval half width */
//         printf("\nbased upon %ld data points", n);
//         printf(" and with %d%% confidence\n", (int)(100.0 * LEVEL_OF_CONFIDENCE + 0.5));
//         printf("the expected value is in the interval");
//         printf("%10.2f +/- %6.2f\n", mean, w);
//     }
//     else
//     {
//         printf("ERROR - insufficient data\n");
//         exit(-9000);
//     }
// }

FILE **create_statistic_files()
{
    const char *centers[] = {
        "ticket_machine",
        "ticket_office",
        "customer_support",
        "security_check",
        "ticket_gate"};

    FILE **files_per_statistic = (FILE **)malloc(sizeof(FILE *) * QUEUE_NUMBER_CENTERS);
    if (files_per_statistic == NULL)
    {
        printf("Error in malloc for fps in create statistics files!\n");
        exit(-1);
    }
    for (int i = 0; i < QUEUE_NUMBER_CENTERS; i++)
    {
        FILE *file_per_centers;

        // Lunghezza del nome del file
        int length = strlen(centers[i]) + strlen(".csv") + 1;
        char *filename = (char *)malloc(length);
        if (filename == NULL)
        {
            printf("Error in malloc for filename in create statistisc file!\n");
            exit(-1);
        }

        // Crea il nome del file
        sprintf(filename, "%s.csv", centers[i]);
        file_per_centers = fopen(filename, "w");
        if (file_per_centers == NULL)
        {
            printf("Error impossible to create file %s.\n", filename);
            fflush(stdout);
            exit(-1);
        }
        free(filename);
        files_per_statistic[i] = file_per_centers;
    }

    return files_per_statistic;
}
