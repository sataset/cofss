#include "utility.h"

void relax_max(double& a, const double& b) {
    if (a < b) a = b;
}

void relax_mix(double& a, const double& b) {
    if (a > b) a = b;
}

double dbm_to_watts(const double& power_dbm) {
    return 1e-3 * std::pow(10, power_dbm / 10);
}