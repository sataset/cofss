#include "tdfa.h"

TDFA::TDFA() {
    name = "tdfa";

    satGain = 0;
    E_satG = 1;

    alpha = 0;
    beta2 = 0;
    gamma = 0;
    length = 1;
    total_steps = 1;
}

TDFA::TDFA(const double& small_signal_gain,
           const double& saturation_power,
           const double& cavity_roundtrip_time) {
    name = "tdfa";

    alpha = 0;
    beta2 = 0;
    gamma = 0;
    length = 1;
    total_steps = 1;

    satGain = small_signal_gain;
    P_satG = saturation_power;
    T_R = cavity_roundtrip_time;
    E_satG = T_R * P_satG;
}

// TDFA::TDFA(const double& small_signal_gain, const double& saturation_energy)
// {
//     name = "tdfa";

//     alpha = 0;
//     beta2 = 0;
//     gamma = 0;
//     length = 1;
//     total_steps = 1;

//     satGain = small_signal_gain;
//     E_satG = saturation_energy;
// }

void TDFA::setAttenuation(const double& in_alpha) { alpha = in_alpha; }

void TDFA::setDispersion(const double& in_beta2) {
    beta2 = in_beta2;
    beta3 = 0;
}

void TDFA::setDispersion(const double& in_beta2, const double& in_beta3) {
    beta2 = in_beta2;
    beta3 = in_beta3;
}

void TDFA::setNonlinearity(const double& in_gamma) { gamma = in_gamma; }

void TDFA::setSatGain(const double& small_signal_gain) {
    satGain = small_signal_gain;
}

void TDFA::setSaturationEnergy(const double& saturation_energy) {
    E_satG = saturation_energy;
}

void TDFA::setFiberLength(const double& in_length) { length = in_length; }

void TDFA::setTotalSteps(const int& steps) { total_steps = steps; }

Field TDFA::estimateLinearity(Field* signal) const {
    double step = length / double(total_steps);
    unsigned long samples = signal->size();

    Field linearity(samples, 0);
    for (unsigned long i = 0; i < samples; ++i) {
        linearity[i] = i_exp(beta2 * 0.5 * signal->w(i) * signal->w(i) * step);
        linearity[i] *= exp(-alpha * step * 0.5);
    }

    return linearity;
}

Field TDFA::estimateLinearity(Field& signal) const {
    double step = length / double(total_steps);
    unsigned long samples = signal.size();
    Field linearity(samples, 0);

    for (unsigned long i = 0; i < samples; ++i) {
        linearity[i] = i_exp(beta2 * 0.5 * signal.w(i) * signal.w(i) * step);
        linearity[i] *= exp(-alpha * step * 0.5);
    }

    return linearity;
}

void TDFA::execute(Field* signal) {
    unsigned long samples = signal->size();
    double step = length / double(total_steps);
    Field linearity = estimateLinearity(signal);

    double energy = signal->average_power() * signal->size() * T_R;
    double gain = sqrt(exp(satGain * step * 0.5 / (1.0 + energy / E_satG)));

    for (unsigned long j = 0; j < samples; ++j)
        (*signal)[j] *= i_exp(gamma * step * norm((*signal)[j]) / 2.0);

    for (unsigned long i = 0; i < total_steps; ++i) {
        signal->fft_inplace();
        (*signal) *= (linearity * exp(gain * step * 0.5));
        signal->ifft_inplace();

        for (unsigned long j = 0; j < samples; ++j)
            (*signal)[j] *= i_exp(gamma * step * norm((*signal)[j]));
    }

    for (unsigned long j = 0; j < samples; ++j)
        (*signal)[j] *= i_exp(-gamma * step * norm((*signal)[j]) / 2.0);

    // Executor::instance()->enqueue(next, signal);
}

#include <iostream>

void TDFA::execute(Polarizations* signal) {
    unsigned long samples = signal->right.size();
    double step = length / double(total_steps);
    Field linearity = estimateLinearity(signal->right);

    double energy =
        (signal->right.average_power() + signal->left.average_power()) *
        signal->right.size() * T_R;
    double gain = sqrt(exp(satGain * step * 0.5 / (1.0 + energy / E_satG)));

    std::vector<double> kappa = {-2. / 3., -4. / 3.};
    double phi_x, phi_y;

    for (unsigned long j = 0; j < samples; ++j) {
        phi_x = kappa[0] * norm(signal->right[j]) +
                kappa[1] * norm(signal->left[j]);
        phi_y = kappa[1] * norm(signal->right[j]) +
                kappa[0] * norm(signal->left[j]);
        signal->right[j] *= i_exp(gamma * 0.5 * step * phi_x);
        signal->left[j] *= i_exp(gamma * 0.5 * step * phi_y);
    }

    for (unsigned long i = 0; i < total_steps; ++i) {
        signal->right.fft_inplace();
        signal->left.fft_inplace();

        (*signal) *= (linearity * exp(gain * step * 0.5));

        signal->right.ifft_inplace();
        signal->left.ifft_inplace();

        for (unsigned long j = 0; j < samples; ++j) {
            phi_x = kappa[0] * norm(signal->right[j]) +
                    kappa[1] * norm(signal->left[j]);
            phi_y = kappa[1] * norm(signal->right[j]) +
                    kappa[0] * norm(signal->left[j]);
            signal->right[j] *= i_exp(gamma * step * phi_x);
            signal->left[j] *= i_exp(gamma * step * phi_y);
        }
    }

    for (unsigned long j = 0; j < samples; ++j) {
        phi_x = kappa[0] * norm(signal->right[j]) +
                kappa[1] * norm(signal->left[j]);
        phi_y = kappa[1] * norm(signal->right[j]) +
                kappa[0] * norm(signal->left[j]);
        signal->right[j] *= i_exp(-gamma * 0.5 * step * phi_x);
        signal->left[j] *= i_exp(-gamma * 0.5 * step * phi_y);
    }

    // Executor::instance()->enqueue(next, signal);
}