#include <time.h>
#include <random>
#include "IsPrime.h"

using namespace std;

const int MIN_NUMBER = 1;
const int FIRST_ODD_PRIME_NUMBER = 3;

bool IsPrime(int number) {
    if (number <= 1) {
        return false; 
    }

    if (number <= FIRST_ODD_PRIME_NUMBER) {
        return true;
    }

    if (number % 2 == 0 || number % 3 == 0) {
        return false;
    }

    for (int i = 5; i * i <= number; i += 6) {
        if (number % i == 0 || number % (i + 2) == 0) {
            return false;
        }
    }

    return true;
}
