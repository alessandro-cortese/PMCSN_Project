#include <math.h>
#include <stdlib.h>
double get_user_arrival_to_web_server(double arrival, double rate)
{
    double p = 0.030331781686;
    double lambda1 = 0.379147271077;
    double lambda2 = 12.120852728923;
    SelectStream(4);
    arrival += generateHyperExponential(p, lambda1, lambda2);
    return (arrival);
}

double generateHyperExponential(double p, double lambda1, double lambda2)
{
    double u = Random();
    if (u < p)
    {
        return -log(1.0 - ((double)rand() / RAND_MAX)) / lambda1;
    }
    else
    {
        return -log(1.0 - ((double)rand() / RAND_MAX)) / lambda2;
    }
}
