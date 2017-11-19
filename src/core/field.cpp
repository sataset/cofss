#include "field.h"

double Field::peak_power() const {
    double power = 0;
    for (unsigned long i = 0; i < size(); ++i)
        if (power < norm(at(i))) power = norm(at(i));

    return power;
}

double Field::average_power() const {
    double power = 0;
    for (unsigned long i = 0; i < size(); ++i)
        power += norm(at(i));

    return power / size();
}

double Field::energy() const
{
    double energy = norm(at(0)) + norm(at(size() - 1));
    energy /= 2.0;
    for (unsigned long i = 1; i < size() - 1; ++i)
        energy += norm(at(i));

    return energy * time_step_;
}

Field Field::chomp(const unsigned long& at_begin,
                   const unsigned long& at_end) const {
    Field chomped(size() - at_begin - at_end);
    for (unsigned long i = 0; i < chomped.size(); ++i) {
        chomped[i] = at(i + at_begin);
    }

    return chomped;
}

Field Field::operator+(const Field& summands) const {
    Field copy(*this);
    if (size() == summands.size())
        for (unsigned long i = 0; i < copy.size(); ++i)
            copy[i] += summands[i];
    else
        throw std::logic_error("fields size mismatch");

    return copy;
}

Field& Field::operator+=(const Field& summands) {
    if (size() == summands.size())
        for (unsigned long i = 0; i < size(); ++i)
            at(i) += summands[i];
    else
        throw std::logic_error("fields size mismatch");
    return *this;
}

Field Field::operator-(const Field& subtrahends) const {
    Field copy(*this);
    if (size() == subtrahends.size())
        for (unsigned long i = 0; i < copy.size(); ++i)
            copy[i] -= subtrahends[i];
    else
        throw std::logic_error("fields size mismatch");
    return copy;
}

Field& Field::operator-=(const Field& subtrahends) {
    if (size() == subtrahends.size())
        for (unsigned long i = 0; i < size(); ++i)
            at(i) -= subtrahends[i];
    else
        throw std::logic_error("fields size mismatch");
    return *this;
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
    else
        throw std::logic_error("fields size mismatch");

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
    else
        throw std::logic_error("fields size mismatch");

    return *this;
}

Field Field::operator/(const Complex& divider) const {
    Field copy(*this);
    for (unsigned long i = 0; i < copy.size(); ++i)
        copy[i] /= divider;

    return copy;
}

// Field Field::operator/(const Field& dividers) const {
//     Field copy(*this);
//     if (size() == dividers.size())
//         for (unsigned long i = 0; i < copy.size(); ++i)
//             copy[i] /= dividers[i];
//     else
//         throw std::logic_error("fields size mismatch");

//     return copy;
// }

Field& Field::operator/=(const Complex& divider) {
    for (unsigned long i = 0; i < size(); ++i)
        at(i) /= divider;
    return *this;
}

void Field::setTimeStep(const double& time_step) {
    time_step_ = time_step;

    unsigned long samples = size();
    time.assign(samples, 0);
    omega.assign(samples, 2.0 * M_PI / time_step_ / samples);

    for (unsigned long i = 0; i < samples; ++i)
        time[i] = double(i) * time_step_;

    for (unsigned long i = 0; i <= samples / 2; ++i)
        omega[i] *= double(i);

    for (unsigned long i = samples / 2 + 1; i < samples; ++i) {
        omega[i] *= double(i) - double(samples);
    }
}

void Field::setSamplingRate(const double& rate) {
    time_step_ = 1.0 / rate;
    unsigned long samples = size();
    time.assign(samples, 0);
    omega.assign(samples, 2.0 * M_PI / time_step_ / samples);

    for (unsigned long i = 0; i < samples; ++i)
        time[i] = double(i) * time_step_;

    for (unsigned long i = 0; i <= samples / 2; ++i)
        omega[i] *= double(i);

    for (unsigned long i = samples / 2 + 1; i < samples; ++i) {
        omega[i] *= double(i) - double(samples);
    }
}

double Field::getSamplingRate() const { return 1.0 / time_step_; }

double Field::dt() const { return time_step_; }

double Field::df() const { return 1.0 / time_step_ / size(); }

double Field::dw() const { return 2.0 * M_PI / time_step_ / size(); }

double Field::t(const unsigned long& i) const { return time[i]; }

double Field::f(const unsigned long& i) const {
    return omega[i] / (2.0 * M_PI);
}

double Field::w(const unsigned long& i) const { return omega[i]; }

RealVector Field::time_grid() const { return time; }

RealVector Field::frequency_grid() const {
    RealVector copy(omega.size(),0);
    unsigned long half_size = omega.size() / 2;
    for (unsigned long i = 0; i < half_size - 1; ++i)
        copy[i] = omega[i + half_size + 1];
    for (unsigned long i = half_size - 1; i < omega.size(); ++i)
        copy[i] = omega[i - half_size + 1];

    return copy;
}

RealVector Field::temporal_power() const {
    RealVector power(size(), 0);
    for (unsigned long i = 0; i < size(); ++i)
        power[i] = norm(at(i));

    return power;
}

RealVector Field::spectral_power() const {
    RealVector power(size(), 0);
    Field copy = *this;
    copy.fft_inplace();
    for (unsigned long i = 0; i < size(); ++i)
        power[i] = norm(copy[i]);

    return power;
}

Field Field::fft() const {
    Field transformed = *this;
    fftw_plan complex_inplace =
        fftw_plan_dft_1d(int(size()),
                         reinterpret_cast<fftw_complex*>(transformed.data()),
                         reinterpret_cast<fftw_complex*>(transformed.data()),
                         FFTW_FORWARD,
                         FFTW_ESTIMATE);
    fftw_execute(complex_inplace);
    fftw_destroy_plan(complex_inplace);

    transformed *= 1.0 / double(size());
    return transformed;
}

Field Field::ifft() const {
    Field transformed = *this;
    fftw_plan complex_inplace =
        fftw_plan_dft_1d(int(size()),
                         reinterpret_cast<fftw_complex*>(transformed.data()),
                         reinterpret_cast<fftw_complex*>(transformed.data()),
                         FFTW_BACKWARD,
                         FFTW_ESTIMATE);
    fftw_execute(complex_inplace);
    fftw_destroy_plan(complex_inplace);
    return transformed;
}

Field& Field::fft_inplace() {
    fftw_plan complex_inplace =
        fftw_plan_dft_1d(int(size()),
                         reinterpret_cast<fftw_complex*>(data()),
                         reinterpret_cast<fftw_complex*>(data()),
                         FFTW_FORWARD,
                         FFTW_ESTIMATE);
    fftw_execute(complex_inplace);
    fftw_destroy_plan(complex_inplace);

    for (unsigned long i = 0; i < size(); ++i)
        at(i) /= size();
    return *this;
}

Field& Field::ifft_inplace() {
    fftw_plan complex_inplace =
        fftw_plan_dft_1d(int(size()),
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
    unsigned long half_size = size() / 2;
    for (unsigned long i = 0; i < half_size; ++i) {
        buffer = at(i);
        at(i) = at(i + half_size);
        at(i + half_size) = buffer;
    }

    return *this;
}

Field Polarizations::x() { return (right + left) / sqrt(2); }

Field Polarizations::y() { return (right - left) / Complex(0, sqrt(2)); }

Polarizations Polarizations::operator*(const Complex& multiplier) const {
    return Polarizations{right * multiplier, left * multiplier};
}

Polarizations Polarizations::operator*(const Field& multipliers) const {
    return Polarizations{right * multipliers, left * multipliers};
}

Polarizations& Polarizations::operator*=(const Complex& multiplier) {
    right *= multiplier;
    left *= multiplier;

    return *this;
}

Polarizations& Polarizations::operator*=(const Field& multipliers) {
    right *= multipliers;
    left *= multipliers;

    return *this;
}

Polarizations& Polarizations::fft_inplace() {
    right.fft_inplace();
    left.fft_inplace();
    return *this;
}
Polarizations& Polarizations::ifft_inplace() {
    right.ifft_inplace();
    left.ifft_inplace();
    return *this;
}

Field convolution(const Field& x, const Field& y) {
    Field z(x.size() + y.size() - 1);
    for (unsigned long i = 0; i < x.size(); ++i)
        for (unsigned long j = 0; j < y.size(); ++j)
            z[i + j] += x[i] * y[j];

    return z;
}
