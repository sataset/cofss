#include <fstream>
#include <iostream>
#include <string>

#include "src/core.h"
#include "src/modulations.h"
#include "src/modules_pack.h"

void output_state(std::ostream& os, Field& signal);
void output_state(std::ostream& os, Polarizations& signal);
void output_state(std::ostream& os, Field* signal);
void output_state(std::ostream& os, Polarizations* signal);

// reference units [km], [ps], [W]
// Length       [km]
// Attenuation  [dB/km]
// Dispersion   [ps/km/km]
// beta_2       [ps^2 / km]
// Nonlinearity [1/W/km]
const double wavelength = 1.885e-9;   // [km] // 1.55e-6;  // [m]
const double pulse_duration = 100.0;  // [ps]
const double time_steps = 8192;
const int fft_steps = 4000;

// Simple Fiber Parameters
const double length = 0.6e-3;  // [km]
const double attenuation =
    db_to_natural(14);             // [dB/km] // needs convert to natural
const double beta_2 = 74;          // [s^2/km]
const double nonlinearity = 0.78;  // [1/W/km]

// Th-Doped Fiber Parameters
const double length_th = 1e-3;
const double attenuation_th = db_to_natural(2.54e3);
const double beta_2_th = 76;
const double nonlinearity_th = 0.78;

const double satGain = db_to_natural(40.0 / 1e-3);  // [dB/km]
const double refractive_index = 1.45;
const double total_cavity_length = 4.0 * length + length_th;
const double cavity_roundtrip_time =
    total_cavity_length * refractive_index * 1e9 / light_speed;
const double P_satG = 0.03;
const double E_satG = cavity_roundtrip_time * P_satG;

// DWNT-SA
const double alpha_0 = 0.64;
const double alpha_ns = 0.36;
const double P_sat = 10;  // [W]

// Plates parameter
const double psi = 0.7, xi = 0.05;

int main() {
    std::ofstream logs("logs.csv", std::ofstream::out | std::ofstream::trunc);
    logs << "#|E_+|,\t|E_-|" << std::endl;

    Fiber* fiber = new Fiber();
    TDFA* tdfa = new TDFA(satGain, E_satG);
    DWNT* dwnt = new DWNT(alpha_0, P_sat, alpha_ns);
    HWP_QWP* plates = new HWP_QWP(psi, xi);
    PD_ISO* pbs = new PD_ISO();
    Logger* logger = new Logger();
    Coupler* coupler = new Coupler(logger);

    coupler->setOuput(logger);

    fiber->setAttenuation(attenuation);
    fiber->setDispersion(beta_2);
    fiber->setNonlinearity(nonlinearity);
    fiber->setFiberLength(length);
    fiber->setTotalSteps(fft_steps);

    tdfa->setAttenuation(attenuation_th);
    tdfa->setDispersion(beta_2_th);
    tdfa->setNonlinearity(nonlinearity_th);
    tdfa->setFiberLength(length_th);
    tdfa->setTotalSteps(fft_steps);

    Polarizations *field = new Polarizations, *sech = new Polarizations;
    *field = {gaussian(time_steps, 10, 10, pulse_duration),
              gaussian(time_steps, 10, 10, pulse_duration)};
    *sech = {sech_pulse(16384, 1), sech_pulse(16384, 1)};

    System sys;
    sys
        //.add(logger)
        .add(tdfa)
        .add(fiber)
        .add(pbs)
        .add(fiber)
        .add(plates)
        .add(fiber)
        .add(coupler)
        .add(fiber)
        .add(dwnt);
        //.add(logger);

    sys.printModules();
    while (sys.getCount() < 3)
        sys.execute(field);

    std::cout << "Propogation finished" << std::endl;
    std::cout << "Getting logs.." << std::endl;
    logger->getFirstNLast(logs);
    //logger->getLogs(logs);
    std::cout << "File successfully saved" << std::endl;

    logs.close();

    return 0;
}

void output_state(std::ostream& os, Field& signal) {
    for (unsigned long i = 0; i < signal.size(); i++)
        os << signal[i].real() << '\t' << signal[i].imag() << '\n';
    os << std::flush;
}

void output_state(std::ostream& os, Polarizations& signal) {
    for (unsigned long i = 0; i < signal.right.size(); i++)
        os << signal.right[i].real() << '\t' << signal.right[i].imag() << '\t'
           << signal.left[i].real() << '\t' << signal.left[i].imag() << '\n';
    os << std::flush;
}

void output_state(std::ostream& os, Field* signal) {
    for (unsigned long i = 0; i < signal->size(); i++)
        os << norm((*signal)[i]) << '\n';
    os << std::flush;
}

void output_state(std::ostream& os, Polarizations* signal) {
    for (unsigned long i = 0; i < signal->right.size(); i++)
        os << norm(signal->right[i]) << '\t' << norm(signal->left[i]) << '\n';
    os << std::flush;
}