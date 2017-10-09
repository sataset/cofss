#include <fstream>
#include <iostream>
#include <string>

#include "lib/fiber.h"
#include "lib/field.h"
#include "lib/system.h"
#include "lib/utility.h"

// reference units [m], [s], [W]
const double distance = 1e5;         // [m]
const double attenuation = 2e-4;     // [dB/m]
const double dispersion = 1.7e-5;    // [s/m/m]
const double wavelength = 1.55e-6;   // [m]
const double nonlinearity = 1.4e-3;  // [1/W/m]
const int pulse_width = 1024;        // [1]

Field sech_pulse(const int& nodes_quantity, const double& width) {
    Field pulse(nodes_quantity, 0);
    double argument;
    for (int i = 0; i < nodes_quantity; i++) {
        argument = double(i) / (nodes_quantity - 1.0);
        argument = 20 * argument - 20 / 2.0;
        pulse[i] = 1.0 / std::cosh(argument / width);
        pulse[i] /= width;
    }

    return pulse;
}

Field rrc_filter(const double& roll_off,
                 const unsigned long& width,
                 const unsigned long& osf) {
    Field filter(width * osf, 0);

    for (unsigned long i = 0; i < width * osf; ++i) {
        double t = double(i) / osf - width / 2.0;

        if (t == 0) {
            filter[i] = 1 - roll_off + 4 * roll_off / M_PI;
        } else if (std::abs(t) == 0.25 / roll_off) {
            filter[i] += (1 + 2 / M_PI) * std::sin(0.25 * M_PI / roll_off);
            filter[i] += (1 - 2 / M_PI) * std::cos(0.25 * M_PI / roll_off);
            filter[i] *= roll_off / std::sqrt(2.0);
        } else {
            filter[i] += std::cos(M_PI * t * (1 + roll_off));
            filter[i] *= 4 * roll_off * t;
            filter[i] += std::sin(M_PI * t * (1 - roll_off));
            filter[i] /= 1 - 16 * roll_off * roll_off * t * t;
            filter[i] /= M_PI * t;
        }
    }

    return filter *= 1.0 / filter.peak_power();
}

void output_state(std::ostream& os, Field& signal) {
    for (unsigned long i = 0; i < signal.size(); i++)
        os << signal[i].real() << '\t' << signal[i].imag() << '\n';
    os << std::flush;
}

void output_state(std::ostream& os, Polarizations& signal) {
    for (unsigned long i = 0; i < signal.x.size(); i++) {
        os << signal.x[i].real() << '\t' << signal.x[i].imag() << '\n';
        os << signal.y[i].real() << '\t' << signal.y[i].imag() << '\n';
    }
    os << std::flush;
}

int main() {
    System main_sys;
    Fiber fiber(wavelength);
    fiber.setAttenuation(db_to_natural(attenuation));
    fiber.setDispersion(disp_to_beta2(dispersion, wavelength));
    fiber.setNonlinearity(nonlinearity);
    fiber.setFiberLength(100000);
    fiber.setTotalSteps(1000);
    fiber.setSamplingRate(32e9);

    main_sys.add(&fiber);
    // rrc_filter(0.01, 1024, 16)
    Polarizations sech = {sech_pulse(16384, 1), sech_pulse(16384, 0.5)};

    main_sys.execute(sech);

    std::ofstream fout_x("out_x.txt"), fout_y("out_y.txt");

    output_state(fout_x, sech);
    output_state(fout_y, sech);
    fout_x.close();
    fout_y.close();
    
    return 0;
}
