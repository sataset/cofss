#include "tdfa.h"

TDFA::TDFA() {
    name = "tdfa";
    gain = db_to_natural(40) / 1e-3;  // [dB/m]
    E_satG = 4.933412967;
}

TDFA::TDFA(const double& small_signal_gain, const double& saturation_energy) {
    name = "tdfa";
    gain = small_signal_gain;
    E_satG = saturation_energy;
}

TDFA::TDFA(const double& small_signal_gain,
           const double& saturation_power,
           const double& cavity_roundtrip_time) {
    gain = small_signal_gain;
    E_satG = saturation_power * cavity_roundtrip_time;
}

void TDFA::execute(Field* signal) {
    double energy = signal->average_power() * signal->size() * signal->dt();
    for (unsigned long i = 0; i < signal->size(); ++i)
        (*signal)[i] *= gain / (1.0 + energy / E_satG);

    // Executor::instance()->enqueue(next, signal);
}

void TDFA::execute(Polarizations* signal) {
    double energy =
        (signal->right.average_power() + signal->left.average_power()) *
        signal->right.size() * signal->right.dt();

    for (unsigned long i = 0; i < signal->right.size(); ++i) {
        signal->right[i] *= gain / (1.0 + energy / E_satG);
        signal->left[i] *= gain / (1.0 + energy / E_satG);
    }

    // Executor::instance()->enqueue(next, signal);
}