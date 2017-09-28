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
    sampling_rate = 0;
    total_steps = 0;
}

void Fiber::setAttenuation(const double& alpha) { this->alpha = alpha; }

void Fiber::setAttenuationDB(const double& alpha_dB) {
    this->alpha = alpha_dB * std::log(10) / 10;
}

// ps / km / km
void Fiber::setDispersionPhysical(const double& dispersion) {
    double w_square = wavelength * wavelength;
    this->beta2 = -w_square * dispersion / (2 * M_PI * light_speed);
}

// ps * ps / km 
void Fiber::setDispersionEngineering(const double& beta2) {
    // beta_2 = - lambda ^ 2 * DPhys / ( 2 pi c )
    this->beta2 = beta2;
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
    for (int i = 0; i < samples; ++i) {
        if (i <= samples / 2)
            freq = freq_step * i;
        else
            freq = freq_step * (i - samples);

        linearity[i] = exp(i_unit * beta2 / 2.0 * freq * freq * step);
        linearity[i] *= exp(-alpha * step / 2.0);
    }

    return linearity;
}

void Fiber::execute(Field& signal) {
    int samples = signal.size();
    double step = fiber_length / total_steps;
    Field linearity = estimateLinearity(samples);

    for (int j = 0; j < samples; ++j)
        signal[j] *= i_exp(i_unit * gamma * step * (norm(signal[j]) / 2));
        // (e^i gamma step ) ^ norm(signal / 2)

    for (int i = 0; i < total_steps; ++i) {
        signal.fft_inplace();
        signal *= linearity;
        signal.ifft_inplace();

        for (int j = 0; j < samples; ++j)
            signal[j] *= i_exp(i_unit * gamma * step * norm(signal[j]));
    }

    for (int j = 0; j < samples; ++j)
        signal[j] *= i_exp(i_unit * gamma * step * (-norm(signal[j]) / 2));
}
