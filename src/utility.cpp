#include "utility.h"

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

Field sech_pulse(const int& nodes_quantity, const double& width) {
    Field pulse(nodes_quantity, 0);
    double argument;
    for (int i = 0; i < nodes_quantity; i++) {
        argument = double(i) / (nodes_quantity - 1.0);
        argument = 20 * argument - 20 / 2.0;
        pulse[i] = 1.0 / std::cosh(argument / width);
        pulse[i] /= width;
    }

    pulse.setSamplingRate(nodes_quantity / 4.0);

    return pulse;
}

Field gaussian(const int& nodes_quantity,
               const double& peak_power,
               const double& fwhm,
               const double& time_window) {
    Field pulse(nodes_quantity, peak_power);
    double step = time_window / nodes_quantity;
    double argument;
    for (int i = 0; i < nodes_quantity; ++i) {
        argument = time_window / 2.0 - double(i) * step;
        pulse[i] *=
            exp(-argument * argument / fwhm / fwhm * 2.0 * log(2.0));
    }
    pulse.setTimeStep(time_window / nodes_quantity);
    return pulse;
}