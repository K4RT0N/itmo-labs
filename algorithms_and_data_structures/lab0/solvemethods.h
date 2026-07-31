#ifndef SOLVEMETHODS_H
#define SOLVEMETHODS_H

void newton_method(double *coeffs, double *roots, int *count, bool *res);
void chord_method(double *coeffs, double *roots, int *count, bool *res);
void chord_newton_method(double *coeffs, double *roots, int *count, bool *res);
void cardano_method(double *coeffs, double *roots, int *count);
void discriminant(double *coeffs, double *roots, int *count);
void linear(double *coeffs, double *roots, int *count);

void horner(double *coeffs, double root);

#endif