#include "logger.h"

Logger::Logger() { name = "logger"; }

void Logger::write_logs_to(std::ostream& os, WriteType wt) {
    unsigned long size = storage.back().right.size();
    os.precision(15);

    if (wt == TIME) {
        RealVector time_domain(storage.front().right.time_grid());
        for (unsigned long i = 0; i < storage.size(); ++i) {
            for (unsigned long j = 0; j < size; ++j)
                os << time_domain[j] << ','
                << storage[i].right[j].real() << ','
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
            os << frequency_domain[j] << ','
            << power_right[j] << ','
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
            os << time_domain[i] << ','
            << storage.front().right[i].real() << ','
            << storage.front().right[i].imag() << ','
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
            os << frequency_domain[j] << ','
            << power_right[j] << ','
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
            os << time_domain[i] << ','
            << storage.back().right[i].real() << ','
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
            os << frequency_domain[j] << ','
            << power_right[j] << ','
            << power_left[j] << std::endl;
        os << '\n' << std::endl;
        return;
    }
}

void Logger::execute(Field* signal) {
    storage.push_back(Polarizations{*signal, Field(signal->size(), 0)});
}

void Logger::execute(Polarizations* signal) { storage.push_back(*signal); }