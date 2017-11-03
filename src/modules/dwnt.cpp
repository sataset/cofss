#include "dwnt.h"

DWNT::DWNT() {
    name = "dwnt-sa";
    alpha_0 = 0.64;   // [%]
    alpha_ns = 0.36;  // [%]
    P_sat = 10;   // [W]
}

DWNT::DWNT(const double& modulation_depth,
           const double& saturation_peak_power,
           const double& non_saturable_losses) {
    name = "dwnt-sa";
    alpha_0 = modulation_depth;
    alpha_ns = non_saturable_losses;
    P_sat = saturation_peak_power;
}

void DWNT::execute(Field* signal) {
    double P;  // |E_+|^2 + |E_-|^2
    for (unsigned long i = 0; i < signal->size(); ++i) {
        P = norm(signal->at(i));
        if ((P = 1.0 - alpha_0 / (1.0 + P / P_sat) - alpha_ns) < 0) P = 0.0;
        signal->at(i) *= P;
    }
}

void DWNT::execute(Polarizations* signal) {
    double P;  // |E_+|^2 + |E_-|^2
    for (unsigned long i = 0; i < signal->right.size(); ++i) {
        P = norm(signal->right[i]) + norm(signal->left[i]);
        if ((P = 1.0 - alpha_0 / (1.0 + P / P_sat) - alpha_ns) < 0) P = 0.0;
        signal->right[i] *= P;
        signal->left[i] *= P;
    }

    // Executor::instance()->enqueue(next, signal);
}