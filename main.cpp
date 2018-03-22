#include <fstream>
#include <iostream>
#include <string>

#include "src/core.h"
#include "src/modules/modules.h"

void output_state(std::ostream& os, Field& signal);
void output_state(std::ostream& os, Polarizations& signal);
void output_state(std::ostream& os, Field* signal);
void output_state(std::ostream& os, Polarizations* signal);
void identification(Polarizations& signal);

// reference units [km], [ps], [W]
// Length       [km]
// Attenuation  [dB/km]
// Dispersion   [ps/km/km]
// beta_2       [ps^2 / km]
// Nonlinearity [1/W/km]
// const double center_wavelength = 1.885e-9;  // [km]
const double center_wavelength_nm = 1885;  // [nm]
const double filter_width = 100.0;         // [nm]
const double pulse_duration = 100.0;       // [ps]
const double time_steps = 8192;
// const double time_step = pulse_duration / time_steps;
const int fft_steps = 10;

// Simple Fiber Parameters
const double length = 0.6e-3;                  // [km]
const double attenuation = db_to_natural(14);  // [dB/km]
const double beta_2 = 74;                      // [ps^2/km]
const double nonlinearity = 0.78;              // [1/W/km]

// Th-Doped Fiber Parameters
const double length_th = 1e-3;                        // [km]
const double attenuation_th = db_to_natural(2.54e3);  // [dB/km]
const double beta_2_th = 76;                          // [ps^2/km]
const double nonlinearity_th = 0.78;                  // [1/W/km]

const double satGain = db_to_natural(40.0 / length_th);  // [dB/km]
const double refractive_index = 1.45;
const double total_cavity_length = 4.0 * length + length_th;  // [km]
const double cavity_roundtrip_time =
    total_cavity_length * refractive_index / light_speed::kmpps;  // [ps]
const double P_satG = 0.04;
// const double E_satG = cavity_roundtrip_time * P_satG;

// DWNT-SA
const double alpha_0 = 0.64;
const double alpha_ns = 0.36;
const double P_sat = 10;  // [W]

// Plates parameter
// const double psi = 0.7 * math_pi, xi = 0.05 * math_pi;

// Initial Gaussian pulse parameters
const double pulse_power = 10.0;
const double pulse_fwhm = 1;

int main(int argc, char* argv[]) {
    // std::string time_logs_s = "logs/time_logs_(psi = ";
    // time_logs_s.append(argv[2]);
    // time_logs_s.append(", chi = ");
    // time_logs_s.append(argv[3]);
    // time_logs_s.append(").csv");

    // std::string freq_logs_s = "logs/freq_logs_(psi = ";
    // freq_logs_s.append(argv[2]);
    // freq_logs_s.append(", chi = ");
    // freq_logs_s.append(argv[3]);
    // freq_logs_s.append(").csv");

    // std::ofstream time_logs(time_logs_s,
    //                         std::ofstream::out | std::ofstream::trunc);
    // std::ofstream freq_logs(freq_logs_s,
    //                         std::ofstream::out | std::ofstream::trunc);

    std::ofstream time_logs("logs/time_logs.csv",
                            std::ofstream::out | std::ofstream::trunc);
    std::ofstream freq_logs("logs/freq_logs.csv",
                            std::ofstream::out | std::ofstream::trunc);

    DWNT* dwnt = new DWNT(alpha_0, P_sat, alpha_ns);
    HWP_QWP* plates =
        new HWP_QWP(atof(argv[2]) * math_pi, atof(argv[3]) * math_pi);
    PD_ISO* pbs = new PD_ISO();

    Logger* coupler_logger = new Logger();
    coupler_logger->setName("coupler_logger");

    Coupler* coupler = new Coupler(coupler_logger);
    coupler->setTransmission(0.5);

    Fiber* fiber = new Fiber();
    fiber->setAttenuation(attenuation);
    fiber->setDispersion(beta_2);
    fiber->setNonlinearity(nonlinearity);
    fiber->setFiberLength(length);
    fiber->setTotalSteps(fft_steps);

    ActiveFiber* tdfa = new ActiveFiber(satGain, P_satG, cavity_roundtrip_time);
    tdfa->setName("tdfa");
    tdfa->setAttenuation(attenuation_th);
    tdfa->setDispersion(beta_2_th);
    tdfa->setNonlinearity(nonlinearity_th);
    tdfa->setFiberLength(length_th);
    tdfa->setTotalSteps(fft_steps);
    tdfa->setCenterWavelength(center_wavelength_nm);
    tdfa->setOmega_0(filter_width);

    // Polarizations* gaussian_pulse = new Polarizations;
    // gaussian_pulse->right =
    //     gaussian(time_steps, pulse_fwhm, pulse_duration / time_steps);
    //*gaussian_pulse *= sqrt(pulse_power);
    Polarizations* lorentzian_pulse = new Polarizations;
    lorentzian_pulse->right =
        lorentzian(time_steps, pulse_fwhm, pulse_duration / time_steps);
    lorentzian_pulse->left =
        lorentzian(time_steps, pulse_fwhm, pulse_duration / time_steps);
    *lorentzian_pulse *= sqrt(pulse_power);


    System sys;
    sys.add(plates)
        .add(fiber)
        .add(coupler)
        .add(fiber)
        .add(dwnt)
        .add(fiber)
        .add(tdfa)
        .add(fiber)
        .add(pbs);

    unsigned long cycles_count = atoi(argv[1]);
    sys.printModules();
    while (sys.getCount() < cycles_count)
        sys.execute(lorentzian_pulse);

    std::cout << "Propogation finished" << std::endl;
    std::cout << "Generating logs.." << std::endl;

    coupler_logger->write_first_to(time_logs, Logger::TIME);
    coupler_logger->write_last_to(time_logs, Logger::TIME);
    coupler_logger->write_first_to(freq_logs, Logger::FREQUENCY);
    coupler_logger->write_last_to(freq_logs, Logger::FREQUENCY);

    // identification(coupler_logger->get_last());

    std::ofstream dlogs("logs/dlogs.csv",
                        std::ofstream::out | std::ofstream::trunc);
    coupler_logger->write_derivative_to(dlogs);

    // coupler_logger->write_logs_to(time_logs, Logger::TIME);
    // coupler_logger->write_logs_to(freq_logs, Logger::FREQUENCY);

    std::cout << "File successfully saved" << std::endl;

    time_logs.close();
    freq_logs.close();

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

void identification(Polarizations& signal) {
    unsigned long size = signal.right.size();
    double time_step = signal.right.getTimeStep();
    // Power right
    RealVector power, dpower(size);
    power = signal.right.temporal_power();

    // 0 step: building derivative function
    dpower[0] = (power[1] - power[0]) / time_step;
    dpower[size - 1] = (power[size - 1] - power[size - 2]) / time_step;
    for (unsigned long i = 1; i < size - 1; ++i)
        dpower[i] = (power[i + 1] - power[i - 1]) / 2.0 / time_step;

    // 1 step: find max
    double range = 0;
    std::pair<unsigned long, double> max_point(0, 0);
    std::vector<std::pair<unsigned long, double> > max_points;

    // 1.1 step: find max dpower to determine error
    for (unsigned long i = 0; i < size; ++i)
        if (dpower[i] > range) range = dpower[i];

    // if power > 10W
    // if dpower is in [-20:20] range
    // if prev dpower is positive
    for (unsigned long i = 1; i < size - 1; ++i)
        if (std::abs(power[i]) > 10.0 && std::abs(dpower[i]) < range / 320.0 &&
            dpower[i - 1] > 0) {
            max_points.push_back(std::pair<unsigned long, double>(i, power[i]));
            if (max_point.second < max_points.back().second)
                max_point = max_points.back();
        }
    // QUESTION: what if we have several space pulses with same energy?
    // 1.2 step: find fwhm of max pulse
    std::pair<unsigned long, unsigned long> fwhm_ij(max_point.first, 0);
    for (unsigned long i = 0; i < size; ++i)
        if (std::abs(power[i] - max_point.second) / 2.0 < 1e-10)
            fwhm_ij.second = i;
}