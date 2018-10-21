#include "fiber.h"

Fiber::Fiber() {
    name = "fiber";
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

Field Fiber::linear_operator(Field* signal, const double& step) const {
    Field linearity(signal->size(), exp(-alpha * step));
    // it's alpha / 2, actually

    for (size_t i = 0; i < signal->size(); ++i)
        linearity[i] *= i_exp(0.5 * beta2 * step * signal->w(i) * signal->w(i));

    return linearity;
}

void Fiber::nonlinear_step(Field* signal, const double& step) {
    for (size_t i = 0; i < signal->size(); ++i)
        signal->at(i) *= i_exp(-gamma * step * norm((*signal)[i]));
}

Polarizations Fiber::nonlinearity(Polarizations* signal) {
    Polarizations nonlinearity = (i_unit * gamma) * (*signal);

    double kappa_1 = -2.0 / 3.0, kappa_2 = -4.0 / 3.0;
    for (size_t i = 0; i < signal->right.size(); ++i) {
        nonlinearity.right[i] *=
            kappa_1 * norm(signal->right[i]) + kappa_2 * norm(signal->left[i]);
        nonlinearity.left[i] *=
            kappa_2 * norm(signal->right[i]) + kappa_1 * norm(signal->left[i]);
    }

    return nonlinearity;
}

void Fiber::nonlinear_step(Polarizations* signal, const double& step) {
    // auto stage = (*signal) + 0.5 * step * nonlinearity(signal);
    // (*signal) += step * nonlinearity(&stage);
    
    // - iγ (A + B) ± iγ/3 (A - B)
    // iγ (κ_1 A + κ_2 B)

    // E = E + Δz N (E + 0.5 * Δz N(E)E) E

    //  I = norm(signal->right[i]) + norm(signal->left[i]);
    //  J = norm(signal->right[i]) - norm(signal->left[i]);
    //  tmp_r = signal->right[i] + 0.5 * step * (gamma * (-I + J / 3.0));
    //  tmp_l = signal->left[i] + 0.5 * step * (gamma * (-I - J / 3.0));
    //  I_ = norm(tmp_r) + norm(tmp_l);
    //  J_ = norm(tmp_r) - norm(tmp_l);
    // signal->right[i] += step * (gamma * (-I_ + J_ / 3.0));
    // signal->left[i] += step * (gamma * (-I_ - J_ / 3.0));

    double I, J;
    double kappa_1 = -2.0 / 3.0, kappa_2 = -4.0 / 3.0;
    for (size_t i = 0; i < signal->right.size(); ++i) {
        signal->right[i] *= i_exp(step * gamma *
                                  (kappa_1 * norm(signal->right[i]) +
                                   kappa_2 * norm(signal->left[i])));
        signal->left[i] *= i_exp(step * gamma *
                                 (kappa_2 * norm(signal->right[i]) +
                                  kappa_1 * norm(signal->left[i])));
        // I = norm(signal->right[i]) + norm(signal->left[i]);
        // J = norm(signal->right[i]) - norm(signal->left[i]);
        // signal->right[i] *= i_exp(step * gamma * (-I + J / 3.0));
        // signal->left[i] *= i_exp(step * gamma * (-I - J / 3.0));
    }
}

void Fiber::execute(Field* signal) {
    double step = length / double(total_steps);

    Field linearity = linear_operator(signal, step);

    nonlinear_step(signal, 0.5 * step);

    for (int i = 0; i < total_steps - 1; ++i) {
        signal->fft_inplace();
        (*signal) *= linearity;
        signal->ifft_inplace();

        nonlinear_step(signal, step);
    }

    signal->fft_inplace();
    (*signal) *= linearity;
    signal->ifft_inplace();

    nonlinear_step(signal, 0.5 * step);

    // Executor::instance()->enqueue(next, signal);
}

void Fiber::execute(Polarizations* signal) {
    double step = length / double(total_steps);

    Field linearity = linear_operator(&(signal->right), step);

    nonlinear_step(signal, 0.5 * step);

    for (int i = 0; i < total_steps - 1; ++i) {
        signal->fft_inplace();
        (*signal) *= linearity;
        signal->ifft_inplace();

        nonlinear_step(signal, step);
    }

    signal->fft_inplace();
    (*signal) *= linearity;
    signal->ifft_inplace();

    nonlinear_step(signal, 0.5 * step);

    // Executor::instance()->enqueue(next, signal);
}
