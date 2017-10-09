#ifndef UTILITY_H_
#define UTILITY_H_

#include <cmath>

const double math_pi = 3.141592653589793;
const double planck = 6.62607004081e-34;
const double light_speed = 299792458;

void relax_max(double& a, const double& b);
void relax_mix(double& a, const double& b);
double dbm_to_watts(const double& power_dbm);
double db_to_natural(const double& db_value);
double disp_to_beta2(const double& dispersion, const double& wavelength);

#endif  // UTILITY_H_
