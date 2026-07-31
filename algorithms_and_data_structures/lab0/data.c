#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#define ABS(X) (((X) < 0) ? (-X) : (X))

double f(double *coeffs, double x) {
    return coeffs[0]*x*x*x*x*x*x + coeffs[1]*x*x*x*x*x + coeffs[2]*x*x*x*x + 
           coeffs[3]*x*x*x + coeffs[4]*x*x + coeffs[5]*x + coeffs[6];
}
double df(double *coeffs, double x) {
    return 6*coeffs[0]*x*x*x*x*x + 5*coeffs[1]*x*x*x*x + 4*coeffs[2]*x*x*x + 
           3*coeffs[3]*x*x + 2*coeffs[4]*x + coeffs[5];
}
double ddf(double *coeffs, double x) {
    return 30*coeffs[0]*x*x*x*x + 20*coeffs[1]*x*x*x + 12*coeffs[2]*x*x + 
           6*coeffs[3]*x + 2*coeffs[4];
}

/*
 * Checks if first line in stdin is clear (only contains space characters)
 * and empties it. Can only be called if it is guaranteed that there is '\n'
 * in stdin.
 */
static bool is_stdin_clear(void) {
    char ch;

    while((ch = getchar()) != '\n')
        if(!isspace(ch)) {
            while(getchar() != '\n')
                continue;
            return false;
        }

    return true;
}

/*
 * Provides the number of iterations from the user
 */
void get_iters(int *iters) {
    /*
     * First attempt to read the number
     */
    printf("Enter your desired number of iterations: ");
    scanf("%d", iters);
    
    /*
     * Invalid entry format (trash at the end of line).
     * Loop of tries.
     */
    while(!is_stdin_clear() || *iters <= 0) {
        printf("Invalid entry format.\nTry again: ");
        scanf("%d", iters);
    }
}

/*
 * Provides the epsilon value from the user
 */
void get_epsilon(double *epsilon) {
    /*
     * First attempt to read the number
     */
    printf("Enter your desired value of error: ");
    scanf("%lf", epsilon);
    
    /*
     * Invalid entry format (trash at the end of line).
     * Loop of tries.
     */
    while(!is_stdin_clear() || *epsilon < 0) {
        printf("Invalid entry format.\nTry again: ");
        scanf("%lf", epsilon);
    }
}

/*
 * Provides the interval from the user
 */
void get_interval(double *interval) {
#   ifdef DEBUG
    fprintf(stderr, "\n\nInterval initialization started\n");
#   endif
    /*
     * First attempt to read interval
     */
    printf("Enter starting and ending coordinates: ");
    scanf("%lf%lf", 
          &interval[0],
          &interval[1]
         );
    
    /*
     * Invalid entry format (not all numbers were succesfully read,
     * trash at the end of line). Loop of tries.
     */
    while(!is_stdin_clear() ||
            interval[0] >= interval[1]) {
        printf("Invalid entry format.\nTry again: ");
        scanf("%lf%lf", 
              &interval[0],
              &interval[1]
             );
    }
#   ifdef DEBUG
    fprintf(stderr,
            "interval = [%lf; %lf]\n",
            interval[0],
            interval[1]
           );
    fprintf(stderr, "Interval initialization ended.\n\n\n");
#   endif
}

/*
 * Tries to calculate interval for chord method. If doesn't succed, returns
 * [0;1]. Output must be checked.
 */
void calc_interval(double *coeffs,double *interval,int iters,double epsilon) {
#   ifdef DEBUG
    fprintf(stderr, "\nInterval calculation started.\n");
#   endif
    for(double i = 0; i <= iters; i += epsilon) {
        /*
         * Success
         */
        if(f(coeffs, i) * f(coeffs, i + epsilon) < 0) {
            interval[0] = i;
            interval[1] = i + epsilon;
#           ifdef DEBUG
            fprintf(stderr, "The new interval is [%lf; %lf]\n",i,i+epsilon);
            fprintf(stderr,"Interval calculation ended.\n\n");
#           endif
            return;
        } else if(f(coeffs, i - epsilon/2) * f(coeffs, i + epsilon/2) < 0) {
            interval[0] = i - epsilon/2;
            interval[1] = i + epsilon/2;
#           ifdef DEBUG
            fprintf(stderr, "The new interval is [%lf; %lf]\n",
                    i - epsilon / 2, i + epsilon / 2);
            fprintf(stderr,"Interval calculation ended.\n\n");
#           endif
            return;
        }
    }
    for(double i = epsilon; i >= -iters; i -= epsilon) {
        /*
         * Success
         */
        if(f(coeffs, i - epsilon) * f(coeffs, i) < 0) {
            interval[0] = i - epsilon;
            interval[1] = i;
#           ifdef DEBUG
            fprintf(stderr, "The new interval is [%lf; %lf]\n",i-epsilon,i);
            fprintf(stderr,"Interval calculation ended.\n\n");
#           endif
            return;
        } else if(f(coeffs, i - epsilon / 2)*f(coeffs, i + epsilon / 2) < 0) {
            interval[0] = i - epsilon / 2;
            interval[1] = i + epsilon / 2;
#           ifdef DEBUG
            fprintf(stderr, "The new interval is [%lf; %lf]\n",
                    i - epsilon / 2, i + epsilon / 2);
            fprintf(stderr,"Interval calculation ended.\n\n");
#           endif
            return;
        }
    }
    /*
     * Failure
     */
    interval[0] = 0;
    interval[1] = 1;
#   ifdef DEBUG
    fprintf(stderr,"Invalid interval returned.\n");
    fprintf(stderr,"Interval calculation ended.\n\n");
#   endif
}

/*
 * gets coefficients from stdin
 */
void get_coeffs(double *coeffs) {
#   ifdef DEBUG
    fprintf(stderr, "\n\nCoefficiens initialization started.\n");
#   endif
    /*
     * First attempt to read coefficients
     */
    printf("Enter coefficients: ");
    scanf("%lf%lf%lf%lf%lf%lf%lf", 
          &coeffs[0],
          &coeffs[1],
          &coeffs[2],
          &coeffs[3],
          &coeffs[4],
          &coeffs[5],
          &coeffs[6]
         );
    
    /*
     * Invalid entry format (not all numbers were succesfully read,
     * trash at the end of line). Loop of tries.
     */
    while(!is_stdin_clear() || isinf(coeffs[0]) || isinf(coeffs[1]) || isinf(coeffs[2]) ||
                               isinf(coeffs[3]) || isinf(coeffs[4]) || isinf(coeffs[5]) ||
                               isinf(coeffs[6])) {
        printf("Invalid entry format.\nTry again: ");
        scanf("%lf%lf%lf%lf%lf%lf%lf", 
              &coeffs[0],
              &coeffs[1],
              &coeffs[2],
              &coeffs[3],
              &coeffs[4],
              &coeffs[5],
              &coeffs[6]
             );
             
    }
#   ifdef DEBUG
    fprintf(stderr,
            "a = %lf b = %lf c = %lf d = %lf k = %lf n = %lf m = %lf\n",
            coeffs[0],
            coeffs[1],
            coeffs[2],
            coeffs[3],
            coeffs[4],
            coeffs[5],
            coeffs[6]
           );
    fprintf(stderr, "Coefficients initialization ended.\n\n\n");
#   endif
}

void filter(double *roots, int *count) {
    int n = *count - 1;
    for(int i = n - 1; i >= 0; i--) {
        for(int j = i; j < n; j++) {
            if(ABS(roots[i] - roots[j]) < DBL_EPSILON) {
                for(int k = j; k < n; k++) {
                    roots[k] = roots[k+1];
                }
                n--;
            }
        }
    }

    *count = n;
}