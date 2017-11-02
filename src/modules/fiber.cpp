#include "fiber.h"

Fiber::Fiber() {
    name = "fiber";

    wavelength = 1.55e-6;

    alpha = 0;
    beta2 = 0;
    gamma = 0;
    length = 0;
    total_steps = 1;
}

Fiber::Fiber(const double& center_wavelength) {
    name = "fiber";

    wavelength = center_wavelength;

    alpha = 0;
    beta2 = 0;
    gamma = 0;
    length = 0;
    total_steps = 1;
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

void Fiber::setFiberLength(const double& length) { this->length = length; }

void Fiber::setTotalSteps(const int& steps) { this->total_steps = steps; }

Field Fiber::estimateLinearity(Field* signal) const {
    double step = length / double(total_steps);
    double samples = signal->size();

    Field linearity(samples, 0);
    for (unsigned long i = 0; i < samples; ++i) {
        linearity[i] = i_exp(beta2 / 2.0 * signal->w(i) * signal->w(i) * step);
        linearity[i] *= exp(-alpha * step / 2.0);
    }

    return linearity;
}

Field Fiber::estimateLinearity(Field& signal) const {
    double step = length / double(total_steps);
    double samples = signal.size();
    Field linearity(samples, 0);
    
    for (unsigned long i = 0; i < samples; ++i) {
        linearity[i] = i_exp(beta2 / 2.0 * signal.w(i) * signal.w(i) * step);
        linearity[i] *= exp(-alpha * step / 2.0);
    }

    return linearity;
}

void Fiber::execute(Field* signal) {
    unsigned long samples = signal->size();
    double step = length / double(total_steps);
    Field linearity = estimateLinearity(signal);
    
    for (unsigned long j = 0; j < samples; ++j)
        (*signal)[j] *= i_exp(gamma * step * norm((*signal)[j]) / 2.0);

    for (unsigned long i = 0; i < total_steps; ++i) {
        signal->fft_inplace();
        (*signal) *= linearity;
        signal->ifft_inplace();

        for (unsigned long j = 0; j < samples; ++j)
            (*signal)[j] *= i_exp(gamma * step * norm((*signal)[j]));
    }

    for (unsigned long j = 0; j < samples; ++j)
        (*signal)[j] *= i_exp(-gamma * step * norm((*signal)[j]) / 2.0);

    //Executor::instance()->enqueue(next, signal);
}

void Fiber::execute(Polarizations* signal) {
    unsigned long samples = signal->right.size();
    double step = length / total_steps;
    Field linearity = estimateLinearity(signal->right);

    std::vector<double> kappa = {-2. / 3., -4. / 3.};
    double phi_x, phi_y;

    for (unsigned long j = 0; j < samples; ++j) {
        phi_y = kappa[1] * norm(signal->right[j]) +
                kappa[0] * norm(signal->left[j]);
        phi_x = kappa[0] * norm(signal->right[j]) +
                kappa[1] * norm(signal->left[j]);
        signal->right[j] *= i_exp(gamma * 0.5 * step * phi_x);
        signal->left[j] *= i_exp(gamma * 0.5 * step * phi_y);
    }

    for (unsigned long i = 0; i < total_steps; ++i) {
        signal->right.fft_inplace();
        signal->left.fft_inplace();

        (*signal) *= linearity;

        signal->right.ifft_inplace();
        signal->left.ifft_inplace();

        for (unsigned long j = 0; j < samples; ++j) {
            phi_y = kappa[1] * norm(signal->right[j]) +
                    kappa[0] * norm(signal->left[j]);
            phi_x = kappa[0] * norm(signal->right[j]) +
                    kappa[1] * norm(signal->left[j]);
            signal->right[j] *= i_exp(gamma * step * phi_x);
            signal->left[j] *= i_exp(gamma * step * phi_y);
        }
    }
    
    for (unsigned long j = 0; j < samples; ++j) {
        phi_y = kappa[1] * norm(signal->right[j]) +
                kappa[0] * norm(signal->left[j]);
        phi_x = kappa[0] * norm(signal->right[j]) +
                kappa[1] * norm(signal->left[j]);
        signal->right[j] *= i_exp(-gamma * 0.5 * step * phi_x);
        signal->left[j] *= i_exp(-gamma * 0.5 * step * phi_y);
    }

    //Executor::instance()->enqueue(next, signal);
}
