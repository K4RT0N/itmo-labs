#ifndef DATA_H
#define DATA_H

double f(double *coeffs, double x);
double df(double *coeffs, double x);
double ddf(double *coeffs, double x);

void calc_interval(double *coeffs,double *interval,int iters,double epsilon);
bool is_stdin_clear(void);
void filter(double *roots, int *count);

void get_interval(double *interval);
void get_epsilon(double *epsilon);
void get_coeffs(double *coeffs);
void get_iters(int *iters);

#endif