#include "modulations.h"

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
               const double& time_width,
               const double& total_width) {
    Field pulse(nodes_quantity, peak_power);
    double step = time_width / nodes_quantity;
    double argument;
    for (int i = 0; i < nodes_quantity; ++i) {
        argument = total_width / 2.0 - total_width * i / nodes_quantity;
        pulse[i] *=
            exp(-argument * argument * 4.0 * log(2) / time_width / time_width);
    }
    pulse.setSamplingRate(time_width / nodes_quantity);
    return pulse;
}