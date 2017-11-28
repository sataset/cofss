#include "dwnt.h"

DWNT::DWNT() {
    name = "dwnt-sa";
    alpha_0 = 0.64;   // [%]
    alpha_ns = 0.36;  // [%]
    P_sat = 10;       // [W]
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
    double absorption;  // |E_+|^2 + |E_-|^2
    int size = signal->size();
    for (int i = 0; i < size; ++i) {
        absorption = alpha_ns + alpha_0 / (1.0 + norm(signal->at(i)) / P_sat);
        if (absorption > 1.0) absorption = 1.0;
        signal->at(i) *= sqrt(1.0 - absorption);
    }
}

void DWNT::execute(Polarizations* signal) {
    execute(&signal->right);
    execute(&signal->left);
}