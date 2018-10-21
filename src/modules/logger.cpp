#include "logger.h"

Logger::Logger() { name = "logger"; }

void Logger::setDelimeter(char delimeter) {
    delimeter_is_defined = true;
    delimeter_ = delimeter;
}

unsigned long Logger::size() { return storage.size(); }

void Logger::clean() { storage.clear(); }

std::vector<Polarizations>& Logger::getStorage() { return storage; }

Polarizations& Logger::get_first() { return storage.front(); }

Polarizations& Logger::get_last() { return storage.back(); }

void Logger::export_metadata(std::ostream& os) {
    if (!delimeter_is_defined) {
        delimeter_ = ',';
        delimeter_is_defined = true;
    }
    os.precision(15);
    os << "# \"delim\" time_steps time_step" << std::endl;
    os << "# \"" << delimeter_ << "\" " << storage.back().right.size() << " "
       << storage.back().right.getTimeStep() << std::endl;
}

void Logger::export_to_heatmap(std::ostream& os) {
    if (!delimeter_is_defined) {
        delimeter_ = ',';
        delimeter_is_defined = true;
    }
    unsigned long size = storage.back().right.size();
    os.precision(15);
    RealVector time_domain(storage.front().right.time_grid());
    for (unsigned long i = 0; i < storage.size(); ++i) {
        for (unsigned long j = 0; j < size; ++j) {
            os << std::norm(storage[i].right[j]);
            if (j != size - 1) os << delimeter_;
        }
        os << '\n';
    }
    os << "\n\n";
    for (unsigned long i = 0; i < storage.size(); ++i) {
        for (unsigned long j = 0; j < size; ++j) {
            os << std::norm(storage[i].left[j]);
            if (j != size - 1) os << delimeter_;
        }
        os << '\n' << std::endl;
    }
}

void Logger::export_every_to(int n, std::ostream& os, WriteType wt) {
    if (!delimeter_is_defined) {
        delimeter_ = ',';
        delimeter_is_defined = true;
    }
    unsigned long size = storage.back().right.size();
    os.precision(15);

    if (wt == TIME) {
        RealVector time_domain(storage.front().right.time_grid());
        for (unsigned long i = 0; i < storage.size(); i += (n - 1)) {
            for (unsigned long j = 0; j < size; ++j)
                os << time_domain[j] << delimeter_ << storage[i].right[j].real()
                   << delimeter_ << storage[i].right[j].imag() << delimeter_
                   << storage[i].left[j].real() << delimeter_
                   << storage[i].left[j].imag() << std::endl;
            os << '\n' << std::endl;
            // os << '\n';
        }
        return;
    }

    if (wt == FREQUENCY) {
        RealVector frequency_domain(storage.front().right.frequency_grid());
        RealVector power_right, power_left;

        for (unsigned long i = 0; i < storage.size(); i += n) {
            power_right = fft_shift(storage[i].right.spectral_power());
            power_left = fft_shift(storage[i].left.spectral_power());
            for (unsigned long j = 0; j < size; ++j)
                os << frequency_domain[j] << delimeter_ << power_right[j]
                   << delimeter_ << power_left[j] << std::endl;
            os << '\n' << std::endl;
        }
        return;
    }
}

void Logger::export_first_to(std::ostream& os, WriteType wt) {
    if (!delimeter_is_defined) {
        delimeter_ = ',';
        delimeter_is_defined = true;
    }
    unsigned long size = storage.back().right.size();
    os.precision(15);

    if (wt == TIME) {
        RealVector time_domain(storage.front().right.time_grid());
        for (unsigned long i = 0; i < size; ++i)
            os << time_domain[i] << delimeter_
               << storage.front().right[i].real() << delimeter_
               << storage.front().right[i].imag() << delimeter_
               << storage.front().left[i].real() << delimeter_
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
            os << frequency_domain[j] << delimeter_ << power_right[j]
               << delimeter_ << power_left[j] << std::endl;
        os << '\n' << std::endl;
        return;
    }
}

void Logger::export_last_to(std::ostream& os, WriteType wt) {
    if (!delimeter_is_defined) {
        delimeter_ = ',';
        delimeter_is_defined = true;
    }
    unsigned long size = storage.back().right.size();
    os.precision(15);

    if (wt == TIME) {
        RealVector time_domain(storage.front().right.time_grid());
        for (unsigned long i = 0; i < size; ++i)
            os << time_domain[i] << delimeter_ << storage.back().right[i].real()
               << delimeter_ << storage.back().right[i].imag() << delimeter_
               << storage.back().left[i].real() << delimeter_
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
            os << frequency_domain[j] << delimeter_ << power_right[j]
               << delimeter_ << power_left[j] << std::endl;
        os << '\n' << std::endl;
        return;
    }
}

void Logger::import_from(std::istream& is) {
    clean();
    unsigned long field_size;
    double time_step;
    char skip;
    // # "delim" time_steps time_step storage_size
    // # "," 8192 0.5 2
    is.ignore(std::numeric_limits<std::streamsize>::max(), is.widen('\n'));
    is >> skip >> skip >> delimeter_ >> skip >> field_size >> time_step;
    delimeter_is_defined = true;

    Field init(field_size);
    init.setTimeStep(time_step);

    double re, im;
    unsigned long i = 1;
    unsigned long file_end = std::count(std::istreambuf_iterator<char>(is),
                                        std::istreambuf_iterator<char>(),
                                        '\n');
    is.clear();
    is.seekg(0, std::ios::beg);
    is.ignore(std::numeric_limits<std::streamsize>::max(), is.widen('\n'));
    is.ignore(std::numeric_limits<std::streamsize>::max(), is.widen('\n'));
    do {
        storage.push_back({init, init});
        for (unsigned long j = 0; j < field_size; ++j) {
            is.ignore(std::numeric_limits<std::streamsize>::max(),
                      is.widen(delimeter_));
            is >> re >> delimeter_ >> im >> delimeter_;
            storage.back().right[j] = Complex(re, im);
            is >> re >> delimeter_ >> im;
            storage.back().left[j] = Complex(re, im);
        }
        is.ignore(std::numeric_limits<std::streamsize>::max(), is.widen('\n'));
        is.ignore(std::numeric_limits<std::streamsize>::max(), is.widen('\n'));
        i += field_size + 2;
    } while (i < file_end);
}

void Logger::export_derivative_to(std::ostream& os) {
    if (!delimeter_is_defined) delimeter_ = ',';
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
        os << dsignal_r[i] << delimeter_ << dsignal_l[i] << std::endl;
}

void Logger::execute(Field* signal) {
    storage.push_back({*signal, Field(signal->size(), 0)});
}

void Logger::execute(Polarizations* signal) { storage.push_back(*signal); }