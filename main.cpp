#include <fstream>
#include <iostream>
#include <string>

#include "src/core.h"
#include "src/modulations.h"
#include "src/modules_pack.h"

// reference units [m], [s], [W]

const double refractive_index = 1.45;
const double gain = 40;
const double E_satG = 4.933412967;

const double wavelength = 1.885e-9;  // [km] // 1.55e-6;  // [m]
const double sampling_rate = 4096;
const int fft_steps = 10;

// const double length = 1e5;           // [m]
// const double attenuation = 2e-4;      // [dB/m]
// const double dispersion = 1.7e-5;    // [s/m/m]
// const double beta2 = disp_to_beta2(dispersion, wavelength);    // [s^2/m]
// const double nonlinearity = 1.4e-3;  // [1/W/m]

const double length = 0.6e-3;    // [km]
const double attenuation = 14;  // [dB/km] // needs convert to natural
const double dispersion = 74;   // [s/km/km]
const double beta2 = disp_to_beta2(dispersion, wavelength);  // [s^2/km]
const double nonlinearity = 0.78;                            // [1/W/km]

const double length_th = 1e-3;                                     // [km]
const double attenuation_th = 2.54e3;                               // [dB/km]
const double dispersion_th = 76;                                  // [s/km/km]
const double beta2_th = disp_to_beta2(dispersion_th, wavelength);  // [s^2/km]
const double nonlinearity_th = 0.78;                               // [1/W/km]

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

int main() {
    std::ofstream logs("logs.csv", std::ofstream::out | std::ofstream::trunc);
    logs << "#|E_+|,\t|E_-|" << std::endl;

    Fiber* fiber = new Fiber(wavelength);
    Fiber* Th_fiber = new Fiber(wavelength);
    TDFA* tdfa = new TDFA(db_to_natural(gain) / length_th, E_satG);
    DWNT* dwnt = new DWNT();
    QWP* qwp = new QWP();
    PD_ISO* pbs = new PD_ISO();
    Logger* logger = new Logger();
    Coupler* coupler = new Coupler(logger);
    Counter* counter = new Counter();

    fiber->setAttenuation(db_to_natural(attenuation));
    fiber->setDispersion(beta2);
    fiber->setNonlinearity(nonlinearity);
    fiber->setFiberLength(length);
    fiber->setTotalSteps(fft_steps);

    Th_fiber->setAttenuation(db_to_natural(attenuation_th));
    Th_fiber->setDispersion(beta2_th);
    Th_fiber->setNonlinearity(nonlinearity_th);
    Th_fiber->setFiberLength(length_th);
    Th_fiber->setTotalSteps(fft_steps);

    Polarizations* sech = new Polarizations;
    *sech = {sech_pulse(16384, 1), sech_pulse(16384, 1)};
    sech->right.setSamplingRate(sampling_rate);
    sech->left.setSamplingRate(sampling_rate);

    System sys;
    sys
        .add(tdfa)
        .add(Th_fiber)
        .add(fiber)
        .add(pbs)
        .add(fiber)
        //.add(qwp)
        .add(fiber)
        .add(coupler)
        .add(fiber)
        .add(dwnt);

    coupler->setOuput(logger);

    // output_state(logs, sech);
    // Th_fiber->execute(sech);
    // tdfa->execute(sech);
    // fiber->execute(sech);
    // dwnt->execute(sech);
    // pbs->execute(sech);
    // coupler->execute(sech);
    // qwp->execute(sech);

    // logs << "\n" << std::endl;
    // output_state(logs, sech);

    sys.printModules();
    while (sys.getCount() < 100)
        sys.execute(sech);
    std::cout << "Propogation finished" << std::endl;
    std::cout << "Getting logs.." << std::endl;
    // logger->getFirstNLast(logs);
    logger->getLogs(logs);
    std::cout << "File successfully saved" << std::endl;

    logs.close();

    return 0;
}