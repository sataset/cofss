#ifndef UTILITY_H_
#define UTILITY_H_

#include <cmath>
#include <complex>
#include <random>
#include "core/field.h"

const double math_pi = 3.141592653589793;
const double planck = 6.62607004081e-34;
const double light_speed = 299792458;

double sinc(const double& x);

double db_to_linear(const double& db_value);
double dbm_to_watts(const double& dbm_power);
double db_to_natural(const double& db_value);
double disp_to_beta2(const double& dispersion, const double& wavelength);

Complex i_exp(const double& x);

// FWHM - full width at half maximum

Field sech_pulse(const int& nodes_quantity, const double& width);
Field gaussian(const int& nodes_quantity,
               const double& peak_power,
               const double& fwhm,
               const double& time_window);

#endif  // UTILITY_H_