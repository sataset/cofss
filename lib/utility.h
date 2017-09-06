#ifndef UTILITY_H_
#define UTILITY_H_

#include <cmath>

void relax_max(double& a, const double& b);
void relax_mix(double& a, const double& b);
double dbm_to_watts(const double& power_dbm);

#endif  // UTILITY_H_
