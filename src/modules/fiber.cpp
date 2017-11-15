#include "fiber.h"

Fiber::Fiber() {
    name = "fiber";
    // gain = nullptr;
    alpha = 0;
    beta2 = 0;
    gamma = 0;
    length = 1;
    total_steps = 1;
}

void Fiber::setAttenuation(const double& in_alpha) { alpha = in_alpha; }

void Fiber::setDispersion(const double& in_beta2) {
    beta2 = in_beta2;
    beta3 = 0;
}

void Fiber::setDispersion(const double& in_beta2, const double& in_beta3) {
    beta2 = in_beta2;
    beta3 = in_beta3;
}

void Fiber::setNonlinearity(const double& in_gamma) { gamma = in_gamma; }

void Fiber::setFiberLength(const double& in_length) { length = in_length; }

void Fiber::setTotalSteps(const int& steps) { total_steps = steps; }

Field Fiber::linear_operator(Field* signal) const {
    double step = length / double(total_steps);
    int samples = signal->size();

    Field linearity(samples, 0);
    for (int i = 0; i < samples; ++i) {
        linearity[i] = i_exp(beta2 * step * 0.5 * signal->w(i) * signal->w(i));
        linearity[i] *= exp(-alpha * 0.5 * step);
    }

    return linearity;
}

void Fiber::execute(Field* signal) {
    int samples = signal->size();
    double step = length / double(total_steps);
    Field linearity = linear_operator(signal);

    for (int j = 0; j < samples; ++j)
        (*signal)[j] *= i_exp(gamma * 0.5 * step * norm((*signal)[j]));

    for (int i = 0; i < total_steps - 1; ++i) {
        signal->fft_inplace();
        (*signal) *= linearity;
        signal->ifft_inplace();

        for (int j = 0; j < samples; ++j)
            (*signal)[j] *= i_exp(gamma * step * norm((*signal)[j]));
    }

    signal->fft_inplace();
    (*signal) *= linearity;
    signal->ifft_inplace();

    for (int j = 0; j < samples; ++j)
        (*signal)[j] *= i_exp(gamma * 0.5 * step * norm((*signal)[j]));

    // Executor::instance()->enqueue(next, signal);
}

void Fiber::execute(Polarizations* signal) {
    int samples = signal->right.size();
    double step = length / double(total_steps);
    Field linearity = linear_operator(&(signal->right));

    std::vector<double> kappa = {-2. / 3., -4. / 3.};
    double phi_right, phi_left;

    for (int j = 0; j < samples; ++j) {
        phi_right = kappa[0] * norm(signal->right[j]) +
                kappa[1] * norm(signal->left[j]);
        phi_left = kappa[1] * norm(signal->right[j]) +
                kappa[0] * norm(signal->left[j]);
        signal->right[j] *= i_exp(gamma * 0.5 * step * phi_right);
        signal->left[j] *= i_exp(gamma * 0.5 * step * phi_left);
    }

    for (int i = 0; i < total_steps - 1; ++i) {
        signal->fft_inplace();
        (*signal) *= linearity;
        signal->ifft_inplace();

        for (int j = 0; j < samples; ++j) {
            phi_right = kappa[0] * norm(signal->right[j]) +
                    kappa[1] * norm(signal->left[j]);
            phi_left = kappa[1] * norm(signal->right[j]) +
                    kappa[0] * norm(signal->left[j]);
            signal->right[j] *= i_exp(gamma * step * phi_right);
            signal->left[j] *= i_exp(gamma * step * phi_left);
        }
    }

    signal->fft_inplace();
    (*signal) *= linearity;
    signal->ifft_inplace();

    for (int j = 0; j < samples; ++j) {
        phi_right = kappa[0] * norm(signal->right[j]) +
                kappa[1] * norm(signal->left[j]);
        phi_left = kappa[1] * norm(signal->right[j]) +
                kappa[0] * norm(signal->left[j]);
        signal->right[j] *= i_exp(gamma * 0.5 * step * phi_right);
        signal->left[j] *= i_exp(gamma * 0.5 * step * phi_left);
    }

    // Executor::instance()->enqueue(next, signal);
}
