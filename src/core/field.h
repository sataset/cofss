#ifndef FIELD_H_
#define FIELD_H_

#include <fftw3.h>
#include <cmath>
#include <complex>
#include <stdexcept>
#include <string>
#include <vector>

typedef std::complex<double> Complex;
typedef std::vector<Complex> ComplexVector;
typedef std::vector<double> RealVector;
const Complex i_unit = Complex(0, 1);

class Field : public ComplexVector {
    double time_step_;
    RealVector omega;
    RealVector time;

  public:
    using ComplexVector::vector;

    double peak_power() const;
    double average_power() const;

    Field chomp(const unsigned long& at_begin,
                const unsigned long& at_end) const;

    Field operator+(const Field& summands) const;
    Field& operator+=(const Field& summands);

    Field operator-(const Field& subtrahends) const;
    Field& operator-=(const Field& subtrahends);

    Field operator*(const Complex& multiplier) const;
    Field operator*(const Field& multipliers) const;
    Field& operator*=(const Complex& multiplier);
    Field& operator*=(const Field& multipliers);

    Field operator/(const Complex& divider) const;
    //Field operator/(const Field& dividers) const;
    Field& operator/=(const Complex& divider);

    void setTimeStep(const double& time_step);
    void setSamplingRate(const double& rate);
    double getSamplingRate() const;
    double dt() const;
    double df() const;
    double dw() const;
    double t(const unsigned long& i) const;
    double f(const unsigned long& i) const;
    double w(const unsigned long& i) const;
    RealVector temporal_power() const;
    RealVector spectral_power() const;

    Field fft() const;
    Field ifft() const;
    Field& fft_inplace();
    Field& ifft_inplace();
    Field& fft_shift();
};

struct Polarizations {
    Field right, left;

    Field x();
    Field y();

    Polarizations operator*(const Complex& multiplier) const;
    Polarizations operator*(const Field& multipliers) const;
    Polarizations& operator*=(const Complex& multiplier);
    Polarizations& operator*=(const Field& multipliers);

    Polarizations& fft_inplace();
    Polarizations& ifft_inplace();
};

Field convolution(const Field& x, const Field& y);

#endif  // FIELD_H_
