#include "logger.h"

Logger::Logger() { name = "logger"; }

void Logger::write_logs_to(std::ostream& os, WriteType wt) {
    unsigned long size = storage.back().right.size();
    os.precision(15);

    if (wt == TIME) {
        RealVector time_domain(storage.front().right.time_grid());
        for (unsigned long i = 0; i < storage.size(); ++i) {
            for (unsigned long j = 0; j < size; ++j)
                os << time_domain[j] << ',' << storage[i].right[j].real() << ','
                   << storage[i].right[j].imag() << ','
                   << storage[i].left[j].real() << ','
                   << storage[i].left[j].imag() << std::endl;
            os << '\n' << std::endl;
        }
        return;
    }

    if (wt == FREQUENCY) {
        RealVector frequency_domain(storage.front().right.frequency_grid());
        RealVector power_right, power_left;

        for (unsigned long i = 0; i < storage.size(); ++i) {
            power_right = fft_shift(storage[i].right.spectral_power());
            power_left = fft_shift(storage[i].left.spectral_power());
            for (unsigned long j = 0; j < size; ++j)
                os << frequency_domain[j] << ',' << power_right[j] << ','
                   << power_left[j] << std::endl;
            os << '\n' << std::endl;
        }
        return;
    }
}

void Logger::write_first_to(std::ostream& os, WriteType wt) {
    unsigned long size = storage.back().right.size();
    os.precision(15);

    if (wt == TIME) {
        RealVector time_domain(storage.front().right.time_grid());
        for (unsigned long i = 0; i < size; ++i)
            os << time_domain[i] << ',' << storage.front().right[i].real()
               << ',' << storage.front().right[i].imag() << ','
               << storage.front().left[i].real() << ','
               << storage.front().left[i].imag() << std::endl;
        os << '\n' << std::endl;
        return;
    }

    if (wt == FREQUENCY) {
        RealVector frequency_domain(storage.front().right.frequency_grid());
        RealVector power_right, power_left;
        power_right = fft_shift(storage.front().right.spectral_power());
        power_left = fft_shift(storage.front().left.spectral_power());
        for (unsigned long j = 0; j < size; ++j)
            os << frequency_domain[j] << ',' << power_right[j] << ','
               << power_left[j] << std::endl;
        os << '\n' << std::endl;
        return;
    }
}

void Logger::write_last_to(std::ostream& os, WriteType wt) {
    unsigned long size = storage.back().right.size();
    os.precision(15);

    if (wt == TIME) {
        RealVector time_domain(storage.front().right.time_grid());
        for (unsigned long i = 0; i < size; ++i)
            os << time_domain[i] << ',' << storage.back().right[i].real() << ','
               << storage.back().right[i].imag() << ','
               << storage.back().left[i].real() << ','
               << storage.back().left[i].imag() << std::endl;
        os << '\n' << std::endl;
        return;
    }

    if (wt == FREQUENCY) {
        RealVector frequency_domain(storage.front().right.frequency_grid());
        RealVector power_right, power_left;
        power_right = fft_shift(storage.back().right.spectral_power());
        power_left = fft_shift(storage.back().left.spectral_power());
        for (unsigned long j = 0; j < size; ++j)
            os << frequency_domain[j] << ',' << power_right[j] << ','
               << power_left[j] << std::endl;
        os << '\n' << std::endl;
        return;
    }
}

Polarizations& Logger::get_last() { return storage.back(); }

void Logger::write_derivative_to(std::ostream& os) {
    unsigned long size = storage.back().right.size();
    double time_step = storage.back().right.getTimeStep();

    RealVector p_r, p_l;
    p_r = storage.back().right.temporal_power();
    p_l = storage.back().left.temporal_power();

    RealVector dsignal_l(size), dsignal_r(size);

    dsignal_r[0] = (p_r[1] - p_r[0]) / time_step;
    dsignal_r[size - 1] = (p_r[size - 1] - p_r[size - 2]) / time_step;
    for (unsigned long i = 1; i < size - 1; ++i)
        dsignal_r[i] = (p_r[i + 1] - p_r[i - 1]) / 2.0 / time_step;

    dsignal_l[0] = (p_l[1] - p_l[0]) / time_step;
    dsignal_l[size - 1] = (p_l[size - 1] - p_l[size - 2]) / time_step;
    for (unsigned long i = 1; i < size - 1; ++i)
        dsignal_l[i] = (p_l[i + 1] - p_l[i - 1]) / 2.0 / time_step;

    for (unsigned long i = 0; i < size; ++i)
        os << dsignal_r[i] << ',' << dsignal_l[i] << std::endl;
}

void Logger::execute(Field* signal) {
    storage.push_back(Polarizations{*signal, Field(signal->size(), 0)});
}

void Logger::execute(Polarizations* signal) { storage.push_back(*signal); }