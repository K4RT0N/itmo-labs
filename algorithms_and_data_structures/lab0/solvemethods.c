#include <stdbool.h>
#include <float.h>
#include <stdio.h>
#include <math.h>

#include "solvemethods.h"
#include "data.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ABS(X) (((X) < 0) ? (-X) : (X))

/*
 * Finds root of equation using Newton's method of approximation of roots.
 * Uses bool *res pointer to tell the calling user program about success
 * or failure of approximation.
 */
void newton_method(double *coeffs, double *roots, int *count, bool *res) {
#   ifdef DEBUG
    fprintf(stderr, "\n\nNewton method started.\n");
#   endif
    
    /*
     * Initializing part of the function
     * (iters, epsilon)
     */
    double epsilon = 0.001;
    int iters = 4096;
    
    double a, b;
    char ch;
    
    /*
     * Init number of iterations
     */
    printf("Would you like to change amount of iterations for "
           "Newton's method (%d)? [y/N]\n",
            iters);
    if((ch = getchar()) == 'y' || ch == 'Y') {
        while(getchar() != '\n')
            continue;
        get_iters(&iters);
    } else if(ch != '\n') {
        while(getchar() != '\n')
            continue;
    }
#   ifdef DEBUG
    fprintf(stderr, "iters = %d\n", iters);
#   endif
    
    /*
     * Init epsilon
     */
    printf("Would you like to change the value of error for "
           "Newton's method (%lf)? [y/N]\n",
            epsilon);
    if((ch = getchar()) == 'y' || ch == 'Y') {
        while(getchar() != '\n')
            continue;
        get_epsilon(&epsilon);
    } else if(ch != '\n') {
        while(getchar() != '\n')
            continue;
    }
#   ifdef DEBUG
    fprintf(stderr, "epsilon = %lf\n", epsilon);
#   endif
    
    /*
     * Calculating part of the function
     */
    for(double j = -10; j <= 10; j += 5) {
#       ifdef DEBUG
        fprintf(stderr, "The first approximatino equals %lf.\n", j);
#       endif

        a = j;
        if(ABS(f(coeffs, a)) < DBL_EPSILON) {
            /*
             * Occurs when the first approximation turned out to be the root
             */
    #       ifdef DEBUG
            fprintf(stderr, "The first approximation turned out "
                            "to be the root\n");
            fprintf(stderr, "X = %lf\n", a);
            fprintf(stderr, "Newton method ended.\n\n\n");
    #       endif
            (*count)++;
            roots[*count-1] = a;
            *res = true;
            return;
        }
        for(int i = 1; i <= iters; i++) {
            if(df(coeffs, a) < DBL_EPSILON) {
                /*
                 * Appears when we have reached the extremum,
                 * but not the root.
                 */
    #           ifdef DEBUG
                fprintf(stderr, "Zero division error.\n");
    #           endif
                break;
            }

            b = a - f(coeffs, a) / df(coeffs, a);
            if(ABS(b - a) < epsilon && f(coeffs, a) < DBL_EPSILON) {
                /*
                 * Success
                 */
    #           ifdef DEBUG
                fprintf(stderr, "The root was approximated.\n");
                fprintf(stderr, "X = %lf\n", b);
                fprintf(stderr, "Newton method ended.\n\n\n");
    #           endif
                (*count)++;
                roots[*count-1] = b;
                *res = true;
                return;
            }
            a = b;
        }
    }

    /*
     * Occurs when a root has not been found
     * within the number of iterations.
     */
#   ifdef DEBUG
    fprintf(stderr, "Couldn't calculate the root in %d iterations.\n", iters);
    fprintf(stderr, "Newton method ended.\n\n\n");
#   endif
    *res = false;
    return;
}

/*
 * Finds root of equation using the chord method of approximation of roots.
 * Uses bool *res pointer to tell the calling user program about success
 * or failure of approximation.
 */
void chord_method(double *coeffs, double *roots, int *count, bool *res) {
#   ifdef DEBUG
    fprintf(stderr, "\n\nThe Chord method started\n");
#   endif
    
    double epsilon = 0.001;
    double interval[2];
    int iters = 4096;

    double a, b, c, d;
    
    char ch;
    
    /*
     * Init number of iterations
     */
    printf("Would you like to change amount of iterations "
           "for the chord method (%d)? [y/N]\n",
            iters);
    if((ch = getchar()) == 'y' || ch == 'Y') {
        while(getchar() != '\n')
            continue;
        get_iters(&iters);
    } else if(ch != '\n') {
        while(getchar() != '\n')
            continue;
    }
#   ifdef DEBUG
    fprintf(stderr, "iters = %d\n", iters);
#   endif
    
    /*
     * Init epsilon
     */
    printf("Would you like to change the value of error "
           "for the chord method (%lf)? [y/N]\n",
            epsilon);
    if((ch = getchar()) == 'y' || ch == 'Y') {
        while(getchar() != '\n')
            continue;
        get_epsilon(&epsilon);
    } else if(ch != '\n') {
        while(getchar() != '\n')
            continue;
    }
#   ifdef DEBUG
    fprintf(stderr, "epsilon = %lf\n", epsilon);
#   endif

    /*
     * Init interval
     * Default interval is not shown to user because it's not static and 
     * has to be calculated according to values of iters and epsilon, which 
     * can take some time.
     */
    printf("Would you like to set the interval for the chord method? [y/N]\n");
    if((ch = getchar()) == 'y' || ch == 'Y') {
        while(getchar() != '\n')
            continue;
        get_interval(interval);
    } else {
        if(ch != '\n') {
            while(getchar() != '\n')
                continue;
        }
        calc_interval(coeffs, interval, iters, epsilon);
    }
    if(f(coeffs, interval[0]) * f(coeffs, interval[1]) >= 0) {
        /* 
         * The interval doesn't suit the chord method' requirements
         */
#       ifdef DEBUG
        fprintf(stderr,"The chord method can't deal with invalid interval.\n");
        fprintf(stderr, "The chord method ended.\n\n\n");
#       endif
        *res = false;
        return;
    }

    a = interval[0];
    b = interval[1];
    c = a - (f(coeffs, a) / ((f(coeffs, b) - f(coeffs, a)))) * (b - a);
    for(int i = 1; i < iters; i++) {
        if(f(coeffs, a) * f(coeffs, c) < 0) {
            b = c;
        } else if(f(coeffs, c) * f(coeffs, b) < 0) {
            a = c;
        }
        d = a - (f(coeffs, a) / ((f(coeffs, b) - f(coeffs, a)))) * (b - a);
        if(ABS(d - c) < epsilon) {
            /*
             * Success
             */
#           ifdef DEBUG
            fprintf(stderr, "The root was approximated.\n");
            fprintf(stderr, "X=%lf\n", d);
            fprintf(stderr, "The chord method ended.\n\n\n");
#           endif
            *res = true;
            (*count)++;
            roots[*count-1] = d;
            return;
        } else {
            c = d;
        }
    }
    /*
     * Failure
     */
#   ifdef DEBUG
    fprintf(stderr, "Couldn't approximate the root.\n");
    fprintf(stderr, "The chord method ended.\n\n\n");
#   endif
    *res = false;
}

/*
 * Finds root of equation using the combined Newton chord method of
 * approximation of roots. Uses bool *res pointer to tell the calling
 * user program about success or failure of approximation.
 */
void chord_newton_method(double *coeffs, double *roots, int *count, bool *res) {
#   ifdef DEBUG
    fprintf(stderr, "\n\nThe combined Newton chord method started\n");
#   endif
    
    double epsilon = 0.001;
    double interval[2];
    int iters = 4096;

    double a, b;
    
    char ch;
    
    /*
     * Init number of iterations
     */
    printf("Would you like to change amount of iterations "
           "for the combined Newton chord method (%d)? [y/N]\n",
            iters);
    if((ch = getchar()) == 'y' || ch == 'Y') {
        while(getchar() != '\n')
            continue;
        get_iters(&iters);
    } else if(ch != '\n') {
        while(getchar() != '\n')
            continue;
    }
#   ifdef DEBUG
    fprintf(stderr, "iters = %d\n", iters);
#   endif
    
    /*
     * Init epsilon
     */
    printf("Would you like to change the value of error "
           "for the combined newton chord method (%lf)? [y/N]\n",
            epsilon);
    if((ch = getchar()) == 'y' || ch == 'Y') {
        while(getchar() != '\n')
            continue;
        get_epsilon(&epsilon);
    } else if(ch != '\n') {
        while(getchar() != '\n')
            continue;
    }
#   ifdef DEBUG
    fprintf(stderr, "epsilon = %lf\n", epsilon);
#   endif

    /*
     * Init interval
     * Default interval is not shown to user because it's not static and 
     * has to be calculated according to values of iters and epsilon, which 
     * can take some time.
     */
    printf("Would you like to set the interval for the "
           "combined newton chord method? [y/N]\n");
    if((ch = getchar()) == 'y' || ch == 'Y') {
        while(getchar() != '\n')
            continue;
        get_interval(interval);
    } else if(ch != '\n') {
        while(getchar() != '\n')
            continue;
    } else {
        calc_interval(coeffs, interval, iters, epsilon);
    }
    if(f(coeffs, interval[0]) * f(coeffs, interval[1]) >= 0) {
        /* 
         * The interval doesn't suit the 
         * combined newton chord method requirements
         */
#       ifdef DEBUG
        fprintf(stderr,"The combined Newton chord method can't deal "
                       "with invalid interval.\n");
        fprintf(stderr, "The combined Newton chord method ended.\n\n\n");
#       endif
        *res = false;
        return;
    }

    /*
     * Checking whether interval meets the requirements for the method
     * and initializing starting values for further calculation.
     */
    if(f(coeffs, interval[0]) * ddf(coeffs, interval[1]) > 0) {
        a = interval[0];
        b = interval[1];
    } else if(f(coeffs, interval[1]) * ddf(coeffs, interval[1]) > 0) {
        a = interval[1];
        b = interval[0];
    } else {
        /*
         * Failure. The interval doesn't meet requirements.
         */
        *res = false;
        return;
    }
#   ifdef DEBUG
    fprintf(stderr, "a = %lf, b = %lf\n", a, b);
#   endif

    /*
     * Checking if first interval already meets requirements for
     * the value of error before calculations.
     */
    if(ABS(a - b) < epsilon) {
#       ifdef DEBUG
        fprintf(stderr, "The first interval turned out to satisfy "
                        "requirements for the value of error.\n");
        fprintf(stderr, "X=%lf\n", a);
        fprintf(stderr, "The combined Newton chord method ended.\n\n\n");
#       endif
        *res = true;
        (*count)++;
        roots[*count-1] = a;
        return;
    }

    /*
     * Aproximating part
     */
    for(int i = 1; i <= iters; i++) {
        double x_1, x_2;
        x_1 = a - (b-a) * f(coeffs, a) / (f(coeffs, b) - f(coeffs, a));
        x_2 = a - f(coeffs, a) / df(coeffs, a);
        a = x_2;
        b = x_1;
        if(ABS(a-b) < epsilon) {
            /*
             * Success
             */
#           ifdef DEBUG
            fprintf(stderr, "The approximation succeded.\n");
            fprintf(stderr, "X=%lf\n", a);
            fprintf(stderr, "The combined Newton chord method ended.\n\n\n");
#           endif
            *res = true;
            (*count)++;
            roots[*count-1] = a;
            return;
        }
    }

    /*
     * The approximation failed.
     */
#   ifdef DEBUG
    fprintf(stderr, "The approximation failed.\n");
    fprintf(stderr, "The combined Newton chord method ended.\n\n\n");
#   endif

    *res = false;
    return;
}

/*
 * Reduces the degree of a polynomial based on a known root
 */
void horner(double *coeffs, double root) {
#   ifdef DEBUG
    fprintf(stderr, "\n\nThe horner scheme started.\nOld coefiicients:\n"
                    "a=%lf b=%lf c=%lf d=%lf k=%lf n=%lf m=%lf\n",
            coeffs[0],
            coeffs[1],
            coeffs[2],
            coeffs[3],
            coeffs[4],
            coeffs[5],
            coeffs[6]
           );
#   endif
    double new_coeffs[7];
    new_coeffs[0] = 0;
    new_coeffs[1] = coeffs[0];
    
    /*
     * The algorithm itself
     */
    for(int i = 2; i <= 6; i++) {
        new_coeffs[i] = new_coeffs[i-1] * root + coeffs[i-1];
    }

    /*
     * Rewriting original coefficients
     */
    for(int i = 0; i < 7; i++) {
        coeffs[i] = new_coeffs[i];
    }
#   ifdef DEBUG
    fprintf(stderr, "New coefiicients:\n"
                    "a=%lf b=%lf c=%lf d=%lf k=%lf n=%lf m=%lf\n",
            new_coeffs[0],
            new_coeffs[1],
            new_coeffs[2],
            new_coeffs[3],
            new_coeffs[4],
            new_coeffs[5],
            new_coeffs[6]
           );
    fprintf(stderr, "The Horner scheme ended.\n\n\n");
#   endif
}

/*
 * Finds root of equation using the cordano method of
 * approximation of roots.
 */
void cardano_method(double *coeffs, double *roots, int *count) {
#   ifdef DEBUG
    fprintf(stderr, "\n\nCordano method started.\n");
#   endif
    double a, b, c, d;
    double y1, y2, y3;
    double p, q;
    double Q;

    a = coeffs[3];
    b = coeffs[4];
    c = coeffs[5];
    d = coeffs[6];

    /*
     * Cannonization
     */
    p = (3*a*c - b*b) / (3 * a*a);
    q = (2*b*b*b - 9*a*b*c + 27*a*a*d) / (27*a*a*a);

    /*
     * Solving
     */
    Q = p*p*p/27 + q*q/4;

    if(Q > 0) {
        double alpha, beta;
        alpha = cbrt(-(q/2) + sqrt(Q));
        beta = cbrt(-(q/2) - sqrt(Q));
        y1 = alpha + beta;
        (*count)++;
        roots[*count-1] = y1 - b / 3 / a;
#       ifdef DEBUG
        fprintf(stderr, "Q > 0. One real root.\n"
                        "x=%lf\n"
                        "Cordano method ended.\n\n\n",
                roots[*count-1]);
#       endif
        return;
    } else if(ABS(Q) < DBL_EPSILON) {
        double alpha, beta;
        alpha = cbrt(-(q/2));
        beta = cbrt(-(q/2));
        y1 = alpha + beta;
        y2 = -((alpha + beta) / 2);
        (*count) += 2;
        roots[*count - 1] = y1 - b / 3 / a;
        roots[*count - 2] = y2 - b / 3 / a;
#       ifdef DEBUG
        fprintf(stderr, "Q = 0. Two real roots.\n"
                        "x1=%lf, x2=%fl\n"
                        "Cordano method ended.\n\n\n",
                roots[*count-1], roots[*count-2]);
#       endif
        return;
    } else {
        /*
         * Work with the irreducible case is done through the Viet formula
         */
        (*count) += 3;
        double acos_arg;

        acos_arg = (3.0 * q) / (2.0 * p) * sqrt(-3.0 / p);
        if(acos_arg > 1.0) {
            acos_arg = 1.0;
        } else if(acos_arg < -1.0) {
            acos_arg = -1.0;
        }

        y1 = 2.0 * sqrt(-p / 3.0) * cos(acos(acos_arg) / 3.0);
        y2 = 2.0 * sqrt(-p / 3.0) * cos((acos(acos_arg) + 2.0 * M_PI) / 3.0);
        y3 = 2.0 * sqrt(-p / 3.0) * cos((acos(acos_arg) + 4.0 * M_PI) / 3.0);

        roots[*count - 1] = y1 - b / 3 / a;
        roots[*count - 2] = y2 - b / 3 / a;
        roots[*count - 3] = y3 - b / 3 / a;

#       ifdef DEBUG
        fprintf(stderr, "Q < 0. Three real roots.\n"
                        "x1=%lf, x2=%lf, x3=%lf\n"
                        "Cordano method ended.\n\n\n",
                roots[*count-1], roots[*count-2], roots[*count-3]);
#       endif
    }
    return;
}

/*
 * Solves a quadratic equation using the discriminant
 */
void discriminant(double *coeffs, double *roots, int *count) {
#   ifdef GEBUG
    fprintf(stderr, "\n\nDiscriminant started.\n");
#   endif
    double a, b, c;
    double x1, x2;
    double D;

    a = coeffs[4];
    b = coeffs[5];
    c = coeffs[6];

    D = b*b - 4*a*c;

    if(D > 0) {
        x1 = (-b + sqrt(D)) / (2*a);
        x2 = (-b - sqrt(D)) / (2*a);
        *count += 2;
        roots[*count-1] = x1;
        roots[*count-2] = x2;
#       ifdef DEBUG
        fprintf(stderr, "D > 0. Two real roots.\n"
                        "x1=%lf, x2=%lf\n"
                        "Discriminant ended.\n\n\n",
                roots[*count-1], roots[*count-2]);
#       endif
        return;
    } else if(ABS(D) < DBL_EPSILON) {
        x1 = -(b / (2*a));
        *count += 1;
        roots[*count-1] = x1;
#       ifdef DEBUG
        fprintf(stderr, "D = 0. One real root.\n"
                        "x=%lf\n"
                        "Discriminant ended.\n\n\n",
                roots[*count-1]);
#       endif
        return;
    }
#   ifdef DEBUG
    fprintf(stderr, "D < 0. No roots.\nDiscriminant ended.\n\n\n");
#   endif
    return;
}

/*
 * Solves linear equation
 */
void linear(double *coeffs, double *roots, int *count) {
#   ifdef DEBUG
    fprintf(stderr, "\n\nLinear equation solving started.\n");
#   endif
    double a, b;
    a = coeffs[5];
    b = coeffs[6];
    (*count) += 1;
    roots[*count-1] = -(b / a);
#   ifdef DEBUG
    fprintf(stderr, "x=%lf\nLinear equation solving ended.\n", roots[*count-1]);
#   endif
    return;
}
