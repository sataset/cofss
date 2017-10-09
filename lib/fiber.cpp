#include "fiber.h"

Fiber::Fiber() {
    name = "fiber";

    wavelength = 1.55e-6;

    alpha = 0;
    beta2 = 0;
    gamma = 0;
    fiber_length = 0;
    sampling_rate = 0;
    total_steps = 0;
}

Fiber::Fiber(const double& center_wavelength) {
    name = "fiber";

    wavelength = center_wavelength;

    alpha = 0;
    beta2 = 0;
    gamma = 0;
    fiber_length = 0;
    sampling_rate = 1;
    total_steps = 0;
}

void Fiber::setAttenuation(const double& alpha) { this->alpha = alpha; }

// ps * ps / km
void Fiber::setDispersion(const double& in_beta2) {
    beta2 = in_beta2;
    beta3 = 0;
}

void Fiber::setDispersion(const double& in_beta2, const double& in_beta3) {
    beta2 = in_beta2;
    beta3 = in_beta3;
}

void Fiber::setNonlinearity(const double& gamma) { this->gamma = gamma; }

void Fiber::setFiberLength(const double& length) {
    this->fiber_length = length;
}

void Fiber::setTotalSteps(const int& steps) { this->total_steps = steps; }

void Fiber::setSamplingRate(const double& rate) { this->sampling_rate = rate; }

Field Fiber::estimateLinearity(const int& samples) const {
    double freq;
    double freq_step = 2 * M_PI * sampling_rate / samples;
    double step = fiber_length / total_steps;

    Field linearity(samples, 0);
    for (unsigned long i = 0; i < samples; ++i) {
        if (i <= samples / 2)
            freq = freq_step * i;
        else
            freq = freq_step * (i - samples);

        linearity[i] = i_exp(beta2 / 2.0 * freq * freq * step);
        linearity[i] *= exp(-alpha * step / 2.0);
    }

    return linearity;
}

void Fiber::execute(Field& signal) {
    unsigned long samples = signal.size();
    double step = fiber_length / total_steps;
    Field linearity = estimateLinearity(samples);

    for (unsigned long j = 0; j < samples; ++j)
        signal[j] *= i_exp(gamma * step * (norm(signal[j]) / 2));

    for (unsigned long i = 0; i < total_steps; ++i) {
        signal.fft_inplace();
        signal *= linearity;
        signal.ifft_inplace();

        for (unsigned long j = 0; j < samples; ++j)
            signal[j] *= i_exp(gamma * step * norm(signal[j]));
    }

    for (unsigned long j = 0; j < samples; ++j)
        signal[j] *= i_exp(gamma * step * (-norm(signal[j]) / 2));
}

void Fiber::execute(Polarizations& signal) {
    unsigned long samples = signal.x.size();
    double step = fiber_length / total_steps;
    Field linearity = estimateLinearity(samples);

    std::vector<double> kappa = {-2. / 3., -4. / 3.};
    double phi_x, phi_y;

    for (unsigned long j = 0; j < samples; ++j) {
        phi_y = kappa[1] * norm(signal.x[j]) + kappa[0] * norm(signal.y[j]);
        phi_x = kappa[0] * norm(signal.x[j]) + kappa[1] * norm(signal.y[j]);
        signal.x[j] *= i_exp(gamma * 0.5 * step * phi_x);
        signal.y[j] *= i_exp(gamma * 0.5 * step * phi_y);
    }

    for (unsigned long i = 0; i < total_steps; ++i) {
        signal.x.fft_inplace();
        signal.y.fft_inplace();

        signal *= linearity;

        signal.x.ifft_inplace();
        signal.y.ifft_inplace();

        for (unsigned long j = 0; j < samples; ++j) {
            phi_y = kappa[1] * norm(signal.x[j]) + kappa[0] * norm(signal.y[j]);
            phi_x = kappa[0] * norm(signal.x[j]) + kappa[1] * norm(signal.y[j]);
            signal.x[j] *= i_exp(gamma * step * phi_x);
            signal.y[j] *= i_exp(gamma * step * phi_y);
        }
    }

    for (unsigned long j = 0; j < samples; ++j) {
        phi_y = kappa[1] * norm(signal.x[j]) + kappa[0] * norm(signal.y[j]);
        phi_x = kappa[0] * norm(signal.x[j]) + kappa[1] * norm(signal.y[j]);
        signal.x[j] *= i_exp(-gamma * 0.5 * step * phi_x);
        signal.y[j] *= i_exp(-gamma * 0.5 * step * phi_y);
    }
}

// void Fiber::execute(std::vector<Field>& signal) {
//     int samples = signal.size();
//     double step = fiber_length / total_steps;
//     Field linearity = estimateLinearity(samples);

//     std::vector<double> cappa = {-2./3., -4./3.};

//     for (unsigned long k = 0; k < signal.size(); ++k) {
//         for (unsigned long j = 0; j < samples; ++j)
//             signal[k][j] *= i_exp(gamma * step * (norm(signal[k][j]) / 2));

//         for (unsigned long i = 0; i < total_steps; ++i) {
//             signal[k].fft_inplace();
//             signal[k] *= linearity;
//             signal[k].ifft_inplace();

//             for (unsigned long j = 0; j < samples; ++j)
//                 signal[k][j] *= i_exp(gamma * step * norm(signal[k][j]));
//         }

//         for (unsigned long j = 0; j < samples; ++j)
//             signal[k][j] *= i_exp(gamma * step * (-norm(signal[k][j]) / 2));
//     }
// }