#ifndef FIELD_H_
#define FIELD_H_

#include <fftw3.h>
#include <cmath>
#include <complex>
#include <vector>

typedef std::complex<double> Complex;
typedef std::vector<Complex> ComplexVector;
const Complex i_unit = Complex(0, 1);

class Field : public ComplexVector {
  public:
    using ComplexVector::vector;

	double null_power() const;
    double peak_power() const;
    double peak_power(const int& from, const int& to) const;
    double average_power() const;
    double average_power(const int& from, const int& to) const;
    
    Field chomp(const int& at_begin, const int& at_end) const;
    
    Field& operator*=(const Complex& multiplier);
    Field& operator*=(const Field& multipliers);

    Field& fft_inplace();
    Field& ifft_inplace();
    Field& fft_shift();

    Field& temporal_power() const;
    Field& spectral_power() const;
};

Field convolution(const Field& x, const Field& y);
Complex i_exp(double Re, double Im) const;

#endif  // FIELD_H_
