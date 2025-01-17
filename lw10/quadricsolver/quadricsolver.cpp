#include <cmath>
#include <iostream>
#include "quadricsolver.h"

int SolveQuadratic(double coefficientA, double coefficientB, double coefficientC, double& root1, double& root2)
{
    if (coefficientA == 0)
    {
        return -1;
    }

    double discriminant = coefficientB * coefficientB - 4 * coefficientA * coefficientC;
    if (discriminant < 0)
    {
        return 0;
    }

    if (discriminant == 0)
    {
        root1 = root2 = -coefficientB / (2 * coefficientA);
        return 1;
    }

    root1 = (-coefficientB + sqrt(discriminant)) / (2 * coefficientA);
    root2 = (-coefficientB - sqrt(discriminant)) / (2 * coefficientA);
    return 2;
}
