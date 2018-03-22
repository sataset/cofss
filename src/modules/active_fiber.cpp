#include "active_fiber.h"
#include <iostream>

ActiveFiber::ActiveFiber() {
    name = "ActiveFiber";

    satGain = 0;
    E_satG = 1;

    alpha = 0;
    beta2 = 0;
    gamma = 0;
    length = 1;
    total_steps = 1;
}

ActiveFiber::ActiveFiber(const double& small_signal_gain,
                         const double& saturation_power,
                         const double& cavity_roundtrip_time) {
    name = "ActiveFiber";

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

// ActiveFiber::ActiveFiber(const double& small_signal_gain, const double&
// saturation_energy)
// {
//     name = "ActiveFiber";

//     alpha = 0;
//     beta2 = 0;
//     gamma = 0;
//     length = 1;
//     total_steps = 1;

//     satGain = small_signal_gain;
//     E_satG = saturation_energy;
// }

void ActiveFiber::setAttenuation(const double& in_alpha) { alpha = in_alpha; }

void ActiveFiber::setDispersion(const double& in_beta2) {
    beta2 = in_beta2;
    beta3 = 0;
}

void ActiveFiber::setDispersion(const double& in_beta2,
                                const double& in_beta3) {
    beta2 = in_beta2;
    beta3 = in_beta3;
}

void ActiveFiber::setNonlinearity(const double& in_gamma) { gamma = in_gamma; }

void ActiveFiber::setSatGain(const double& small_signal_gain) {
    satGain = small_signal_gain;
}

void ActiveFiber::setSaturationEnergy(const double& saturation_energy) {
    E_satG = saturation_energy;
}

void ActiveFiber::setFiberLength(const double& in_length) {
    length = in_length;
}

void ActiveFiber::setTotalSteps(const int& steps) { total_steps = steps; }

void ActiveFiber::setCenterWavelength(const double& center_wavelength) {
    center_wavelength_ = center_wavelength;
}

void ActiveFiber::setOmega_0(const double& omega_0) { omega_0_ = omega_0; }

Field ActiveFiber::awgn(const int& samples) const {
    Field noise(samples, 0);
    double phi_1, phi_2;
    double norm = 0.005 / 500.0 / 1e5;

    for (int i = 0; i < samples; ++i) {
        phi_1 = random_value();
        phi_2 = random_value();
        noise[i] =
            sqrt(norm) * sqrt(-2.0 * log(phi_1)) * i_exp(2.0 * math_pi * phi_2);
    }

    return noise;
}

Field ActiveFiber::filtered_gain(Field* field) const {
    double step = length / double(total_steps);
    int samples = field->size();

    double energy = field->energy();
    double gain = satGain / (1.0 + energy / E_satG);

    Field filtered_gain_(samples, 0.0);

    double arg, dw = field->dw();
    double fwhm = 2.0 * math_pi * light_speed::kmps * omega_0_ /
                  center_wavelength_ / center_wavelength_;

    for (int i = 0; i < samples; ++i) {
        arg = dw * double((i - samples / 2));
        filtered_gain_[i] =
            sqrt(exp(step * gain / (1.0 + 4.0 * arg * arg / fwhm / fwhm)));
    }

    return filtered_gain_.fft_shift();
}

Field ActiveFiber::filtered_gain(Polarizations* field) const {
    double step = length / double(total_steps);
    int samples = field->right.size();

    double energy = field->right.energy() + field->left.energy();
    double gain = satGain / (1.0 + energy / E_satG);

    Field filtered_gain_(samples, 0.0);

    double arg, dw = field->right.dw();
    double fwhm = 2.0 * math_pi * light_speed::kmps * omega_0_ /
                  center_wavelength_ / center_wavelength_;

    for (int i = 0; i < samples; ++i) {
        arg = dw * double((i - samples / 2));
        filtered_gain_[i] =
            sqrt(exp(step * gain / (1.0 + 4.0 * arg * arg / fwhm / fwhm)));
    }

    return filtered_gain_.fft_shift();
}

Field ActiveFiber::linear_operator(Field* field) const {
    double step = length / double(total_steps);
    int samples = field->size();

    Field linearity(samples, 0);
    for (int i = 0; i < samples; ++i) {
        linearity[i] = i_exp(beta2 * step * 0.5 * field->w(i) * field->w(i));
        linearity[i] *= exp(-alpha * step);
    }

    return linearity;
}

void ActiveFiber::execute(Field* signal) {
    int samples = signal->size();
    double step = length / double(total_steps);
    Field linearity = linear_operator(signal);
    Field filtered_gain_ = filtered_gain(signal);

    for (int j = 0; j < samples; ++j)
        (*signal)[j] *= i_exp(gamma * 0.5 * step * norm((*signal)[j]));

    for (int i = 0; i < total_steps; ++i) {
        filtered_gain_ = filtered_gain(signal);
        signal->fft_inplace();
        (*signal) *= (linearity * filtered_gain_);
        signal->ifft_inplace();

        for (int j = 0; j < samples; ++j)
            (*signal)[j] *= i_exp(gamma * step * norm((*signal)[j]));
    }

    for (int j = 0; j < samples; ++j)
        (*signal)[j] *= i_exp(-gamma * 0.5 * step * norm((*signal)[j]));

    signal->fft_inplace();
    *signal += awgn(samples);
    signal->ifft_inplace();
}

void ActiveFiber::execute(Polarizations* signal) {
    int samples = signal->right.size();
    double step = length / double(total_steps);

    Field linearity = linear_operator(&(signal->right));
    Field filtered_gain_ = filtered_gain(signal);

    double kappa_1 = -2.0 / 3.0, kappa_2 = -4.0 / 3.0;
    double phi_right, phi_left;

    for (int j = 0; j < samples; ++j) {
        phi_right =
            kappa_1 * norm(signal->right[j]) + kappa_2 * norm(signal->left[j]);
        phi_left =
            kappa_2 * norm(signal->right[j]) + kappa_1 * norm(signal->left[j]);
        signal->right[j] *= i_exp(gamma * 0.5 * step * phi_right);
        signal->left[j] *= i_exp(gamma * 0.5 * step * phi_left);
    }

    for (int i = 0; i < total_steps - 1; ++i) {
        filtered_gain_ = filtered_gain(signal);
        signal->fft_inplace();
        (*signal) *= (linearity * filtered_gain_);
        signal->ifft_inplace();

        for (int j = 0; j < samples; ++j) {
            phi_right = kappa_1 * norm(signal->right[j]) +
                        kappa_2 * norm(signal->left[j]);
            phi_left = kappa_2 * norm(signal->right[j]) +
                       kappa_1 * norm(signal->left[j]);
            signal->right[j] *= i_exp(gamma * step * phi_right);
            signal->left[j] *= i_exp(gamma * step * phi_left);
        }
    }

    filtered_gain_ = filtered_gain(signal);
    signal->fft_inplace();
    (*signal) *= (linearity * filtered_gain_);
    signal->ifft_inplace();

    for (int j = 0; j < samples; ++j) {
        phi_right =
            kappa_1 * norm(signal->right[j]) + kappa_2 * norm(signal->left[j]);
        phi_left =
            kappa_2 * norm(signal->right[j]) + kappa_1 * norm(signal->left[j]);
        signal->right[j] *= i_exp(gamma * 0.5 * step * phi_right);
        signal->left[j] *= i_exp(gamma * 0.5 * step * phi_left);
    }

    signal->fft_inplace();
    signal->right += awgn(samples);
    signal->left += awgn(samples);
    signal->ifft_inplace();
}