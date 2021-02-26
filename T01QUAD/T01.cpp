#include <stdio.h> //for input and output in console
#include <assert.h>//for assert
#include <cmath>   //for sqrt and isfinite

//for double comparisons
#define DELTA 0.0000005

using namespace std;

//{============================================================================
//! For safer natural+0 numbers
//!
//! @note NOT_A_NUMBER = -2 is for NAN
//! @note INF = -1 is for infinite numbers
//}============================================================================

enum number {

    ZERO        ,
    ONE         ,
    TWO         ,

    NOT_A_NUMBER = -2,
    INF

};

//{============================================================================
//! Solves linear equations of the form: ax + b = 0
//!
//! @param [in]  a       a-coefficient
//! @param [in]  b       b-coefficient
//!
//! @param [out] x      pointer for solution
//!
//! @return number of solutions
//!
//! @note in case of infinite number of solutions returns INF_NUM_OF_SOL = -1
//}============================================================================

number solveLinear( double a, double b, double* x );

//{============================================================================
//! Solves quadratic equations of the form: ax^2 + bx + c = 0
//!
//! @param [in]  a       a-coefficient
//! @param [in]  b       b-coefficient
//! @param [in]  c       c-coefficient
//!
//! @param [out] x1      pointer for first solution
//! @param [out] x2      pointer for second solution
//!
//! @return number of solutions
//!
//! @note in case of infinite number of solutions returns INF_NUM_OF_SOL = -1
//}============================================================================

number solveQuadr( double a, double b, double c, double* x1, double* x2 );

//{============================================================================
//! Inputs x in expression of the form: ax^2 + bx + c and returns the result
//!
//! @param [in]  a       a-coefficient
//! @param [in]  b       b-coefficient
//! @param [in]  c       c-coefficient
//!
//! @param [in] x        x
//!
//! @return expression value for current x
//!
//! @note in case of NAN x returns NAN
//}============================================================================

double inputQuadrSol( double a, double b, double c, double x );

int main ()
{
    double a = NAN;
    double b = NAN;
    double c = NAN;

    double x1 = NAN;
    double x2 = NAN;

    enum number numOfSol = NOT_A_NUMBER;

    printf ("Program to solve quadratic equations \nplease, input coefficients:");
    scanf  ("%lf  %lf  %lf", &a, &b, &c);
    printf ("\n");


    numOfSol = solveQuadr (a, b, c, &x1, &x2);

    switch (numOfSol)
    {

        case ZERO:
                            printf ("No solutions!\n");
                            break;

        case ONE:
                            printf ("1 solution: %lf!\n", x1);
                            printf ("expression value: %lf\n", inputQuadrSol(a, b, c, x1));
                            break;

        case TWO:
                            printf ("2 solutions: %lf and %lf!\n", x1, x2);
                            printf ("expression value for 1st solution: %lf\n", inputQuadrSol(a, b, c, x1));
                            printf ("expression value for 2nd solution: %lf\n", inputQuadrSol(a, b, c, x2));
                            break;

        case INF:
                            printf ("solutions are all numbers!\n");
                            break;

        default:
                            printf ("ERROR, smth wrong with num of solutions!\nnumOfSol = %d\n", numOfSol);
                            return 1;

    }

    return 0;
}

number solveLinear( double a, double b, double* x )
{
    assert (isfinite(a));
    assert (isfinite(b));

    assert (x != NULL);

    if (abs (a) < DELTA)
    {
        if (abs (b) < DELTA)//              0 = 0
            return INF;

        return ZERO;//                      b = 0
    }

    *x = -b / a;//                          ax + b = 0
    return ONE;
}

number solveQuadr( double a, double b, double c, double* x1, double* x2 )
{
    assert (isfinite(a));
    assert (isfinite(b));
    assert (isfinite(c));

    assert (x1 != NULL);
    assert (x2 != NULL);
    assert (x1 != x2);

    if (abs (a) < DELTA)
        return solveLinear(b, c, x1);

    if (abs (c) < DELTA)//              x(ax + b) = 0
    {
        *x1 = 0;
        *x2 = -b / a;
        return TWO;
    }

    double D = b * b - 4 * a * c; //    discriminant

    if (D < 0)
        return ZERO;

    if (abs (D) < DELTA)
    {
        *x1 = *x2 = -b / (2 * a);
        return TWO;
    }

    D = sqrt(D);//                      to count once

    *x1 = (-b + D) / (2 * a);
    *x2 = (-b - D) / (2 * a);
    return TWO;
}

double inputQuadrSol( double a, double b, double c, double x )
{
    assert (isfinite(a));
    assert (isfinite(b));
    assert (isfinite(c));

    return a * x * x + b * x + c;
}


