#include "utility.h"

void relax_max(double& a, const double& b) {
    if (a < b) a = b;
}

void relax_mix(double& a, const double& b) {
    if (a > b) a = b;
}

double sinc(const double& x) {
    if (x == 0) return 1;
    return std::sin(M_PI * x) / M_PI / x;
}

double db_to_linear(const double& db_value) {
    return std::pow(10, db_value / 10);
}

double dbm_to_watts(const double& dbm_power) {
    return 1e-3 * std::pow(10, dbm_power / 10);
}

double db_to_natural(const double& db_value) {
    return db_value * std::log(10) / 10;
}

double disp_to_beta2(const double& dispersion, const double& wavelength) {
    return -wavelength * wavelength * dispersion / (2 * math_pi * light_speed);
}

Complex i_exp(const double& x) { return Complex(std::cos(x), std::sin(x)); }