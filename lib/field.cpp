#include "field.h"

double Field::null_power() const { return 0.0; }

double Field::peak_power() const {
    double power = 0;
    for (int i = 0; i < size(); ++i)
        if (power < norm(at(i))) power = norm(at(i));

    return power;
}

double Field::peak_power(const int& from, const int& to) const {
    double power = 0;
    for (int i = from; i < to; ++i)
        if (power < norm(at(i))) power = norm(at(i));

    return power;
}

double Field::average_power() const {
    double power = 0;
    for (int i = 0; i < size(); ++i)
        power += norm(at(i));

    return power / size();
}

double Field::average_power(const int& from, const int& to) const {
    double power = 0;
    for (int i = from; i < to; ++i)
        power += norm(at(i));

    return power / (to - from);
}

Field Field::chomp(const int& at_begin, const int& at_end) const {
    Field chomped(size() - at_begin - at_end);
    for (int i = 0; i < chomped.size(); ++i) {
        chomped[i] = at(i + at_begin);
    }

    return chomped;
}

Field Field::operator*(const Complex& multiplier) const {
    Field copy(*this);
    for (unsigned long i = 0; i < copy.size(); ++i)
        copy[i] *= multiplier;
    return copy;
}

Field Field::operator*(const Field& multipliers) const {
    Field copy(*this);
    if (size() == multipliers.size())
        for (unsigned long i = 0; i < copy.size(); ++i)
            copy[i] *= multipliers[i];

    return copy;
}

Field& Field::operator*=(const Complex& multiplier) {
    for (unsigned long i = 0; i < size(); ++i)
        at(i) *= multiplier;
    return *this;
}

Field& Field::operator*=(const Field& multipliers) {
    if (size() == multipliers.size())
        for (unsigned long i = 0; i < size(); ++i)
            at(i) *= multipliers[i];
    return *this;
}

Field& Field::fft_inplace() {
    fftw_plan complex_inplace =
        fftw_plan_dft_1d(size(),
                         reinterpret_cast<fftw_complex*>(data()),
                         reinterpret_cast<fftw_complex*>(data()),
                         FFTW_FORWARD,
                         FFTW_ESTIMATE);
    fftw_execute(complex_inplace);
    fftw_destroy_plan(complex_inplace);

    for (int i = 0; i < size(); ++i)
        at(i) /= size();

    return *this;
}

Field& Field::ifft_inplace() {
    fftw_plan complex_inplace =
        fftw_plan_dft_1d(size(),
                         reinterpret_cast<fftw_complex*>(data()),
                         reinterpret_cast<fftw_complex*>(data()),
                         FFTW_BACKWARD,
                         FFTW_ESTIMATE);
    fftw_execute(complex_inplace);
    fftw_destroy_plan(complex_inplace);
    return *this;
}

Field& Field::fft_shift() {
    Complex buffer;
    int half_size = size() / 2;
    for (int i = 0; i < half_size; ++i) {
        buffer = at(i);
        at(i) = at(i + half_size);
        at(i + half_size) = buffer;
    }

    return *this;
}

// Field& Field::temporal_power() const {
//     Field temporal(*this);
//     for (int i = 0; temporal.size(); i++)
//         temporal[i] = temporal[i] * temporal[i];

//     return temporal;
// }

// Field& Field::spectral_power() const {
//     Field spectral(*this);
//     spectral.fft_inplace();

//     for (int i = 0; spectral.size(); i++)
//         spectral[i] = spectral[i] * spectral[i];

//     return spectral;
// }

Polarizations Polarizations::operator*(const Complex& multiplier) const {
    return Polarizations{x * multiplier, y * multiplier};
}

Polarizations Polarizations::operator*(const Field& multipliers) const {
    return Polarizations{x * multipliers, y * multipliers};
}

Polarizations& Polarizations::operator*=(const Complex& multiplier) {
    x *= multiplier;
    y *= multiplier;

    return *this;
}

Polarizations& Polarizations::operator*=(const Field& multipliers) {
    x *= multipliers;
    y *= multipliers;

    return *this;
}

Field convolution(const Field& x, const Field& y) {
    Field z(x.size() + y.size() - 1);
    for (int i = 0; i < x.size(); ++i)
        for (int j = 0; j < y.size(); ++j)
            z[i + j] += x[i] * y[j];

    return z;
}

Complex i_exp(const double& phi) { return Complex(cos(phi), sin(phi)); }