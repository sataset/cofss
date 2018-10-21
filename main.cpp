#include <fstream>
#include <iostream>
#include <string>

#include "core.h"
#include "modules/modules.h"

struct peak {
    unsigned long position;
    double power;
    double prominence;

    peak(unsigned long first, double second, double third) {
        position = first;
        power = second;
        prominence = third;
    }
};

void output_state(std::ostream& os, Field& signal);
void output_state(std::ostream& os, Polarizations& signal);
void output_state(std::ostream& os, Field* signal);
void output_state(std::ostream& os, Polarizations* signal);
RealVector smooth(RealVector input);
std::vector<peak> findpeaks(Field signal);
int identification(std::vector<Polarizations>& storage);
void id_response(int id_value, std::ostream& os);

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
int fft_steps = 10;
int number_of_cycles = 2000;

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
double P_satG = 0.03;
// const double E_satG = cavity_roundtrip_time * P_satG;

// DWNT-SA
const double alpha_0 = 0.64;
const double alpha_ns = 0.36;
const double P_sat = 10;  // [W]

// Plates parameter
// psi [0 : π] | chi [0 : π / 4]
double psi = 0.063 * math_pi, chi = 0.1 * math_pi;

// Initial Gaussian pulse parameters
const double pulse_power = 10.0;
const double pulse_fwhm = 1;

//  psi(hwp), chi(qwp), P_satG, fft_steps, cycles, every_N
int main(int argc, char** argv) {
    P_satG = atof(argv[3]);
    fft_steps = atoi(argv[4]);
    number_of_cycles = atoi(argv[5]);
    int every_N = atoi(argv[6]);

    DWNT dwnt(alpha_0, P_sat, alpha_ns);
    HWP_QWP plates(atof(argv[1]) * math_pi, atof(argv[2]) * math_pi);
    PD_ISO pbs;

    Logger coupler_logger;
    coupler_logger.setName("coupler_logger");

    Coupler coupler(&coupler_logger);
    coupler.setTransmission(0.5);

    Fiber fiber;
    fiber.setAttenuation(attenuation);
    fiber.setDispersion(beta_2);
    fiber.setNonlinearity(nonlinearity);
    fiber.setFiberLength(length);
    fiber.setTotalSteps(fft_steps);

    ActiveFiber tdfa(satGain, P_satG, cavity_roundtrip_time);
    tdfa.setName("tdfa");
    tdfa.setAttenuation(attenuation_th);
    tdfa.setDispersion(beta_2_th);
    tdfa.setNonlinearity(nonlinearity_th);
    tdfa.setFiberLength(length_th);
    tdfa.setTotalSteps(fft_steps);
    tdfa.setCenterWavelength(center_wavelength_nm);
    tdfa.setOmega_0(filter_width);

    System sys;
    sys.add(&plates)
        .add(&fiber)
        .add(&coupler)
        .add(&fiber)
        .add(&dwnt)
        .add(&fiber)
        .add(&tdfa)
        .add(&fiber)
        .add(&pbs);
    sys.printModules();

    Polarizations lorentzian_pulse = {
        lorentzian(time_steps, pulse_fwhm, pulse_duration / time_steps),
        lorentzian(time_steps, pulse_fwhm, pulse_duration / time_steps)};
    lorentzian_pulse *= sqrt(pulse_power);

    // plates->change_chi(atof(argv[1]) * math_pi);

    while (sys.getCount() < number_of_cycles)
        sys.execute(&lorentzian_pulse);

    std::cout << "Propogation finished" << std::endl;
    std::cout << "Generating logs.." << std::endl;

    std::string freq_logs_s, time_logs_s, ending_s;
    time_logs_s = "logs/time";
    freq_logs_s = "logs/freq";
    ending_s.append("_logs(");
    ending_s.append(argv[1]);
    ending_s.append(",");
    ending_s.append(argv[2]);
    ending_s.append(").csv");
    time_logs_s.append(ending_s);
    freq_logs_s.append(ending_s);

    std::ofstream time_logs(time_logs_s, std::ofstream::out | std::ofstream::trunc);
    std::ofstream freq_logs(freq_logs_s, std::ofstream::out | std::ofstream::trunc);

    coupler_logger.export_metadata(time_logs);
    coupler_logger.export_every_to(every_N, time_logs, Logger::TIME);
    coupler_logger.export_last_to(freq_logs, Logger::FREQUENCY);
    
    // coupler_logger.export_first_to(time_logs, Logger::TIME);
    // coupler_logger.export_last_to(time_logs, Logger::TIME);
    // coupler_logger.export_first_to(freq_logs, Logger::FREQUENCY);
    // coupler_logger.export_last_to(freq_logs, Logger::FREQUENCY);

    std::cout << "File successfully saved" << std::endl;

    id_response(identification(coupler_logger.getStorage()), std::cout);

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

RealVector smooth(RealVector input) {
    RealVector result(input.size() - 4);

    for (unsigned long i = 0; i < input.size() - 4; ++i)
        result[i] = (input[i] + 4.0 * input[i + 1] + 6.0 * input[i + 2] +
                     4.0 * input[i + 3] + input[i + 4]) /
                    16.0;
    return result;
}

std::vector<struct peak> findpeaks(Field signal) {
    double level = signal.peak_power() / 10.0;
    RealVector power = smooth(signal.temporal_power());

    std::cout << "Finding peaks" << std::endl;
    // Finding peaks
    std::vector<peak> peaks;
    for (unsigned long i = 1; i < power.size() - 1; ++i)
        if (power[i - 1] < power[i] && power[i + 1] < power[i] &&
            power[i] > level)
            peaks.push_back({i, power[i], std::abs(power[i] - power[0])});
    std::cout << "Determining prominence" << std::endl;
    // Determining prominence
    for (unsigned long i = 0; i < peaks.size(); ++i)
        for (unsigned long j = peaks[i].position; j < power.size() - 1; ++j)
            if (power[j - 1] > power[j] && power[j + 1] > power[j]) {
                peaks[i].prominence = std::abs(peaks[i].power - power[j]);
                break;
            }
    std::cout << "Sorting" << std::endl;
    std::sort(peaks.begin(), peaks.end(), [](peak p1, peak p2) {
        return (p1.power < p2.power);
    });
    return peaks;
}

int identification(std::vector<Polarizations>& storage) {
    std::vector<double> energy_on_cycles(100);

    if (storage.size() >= 100)
        for (unsigned long i = 0, size = storage.size() - 1; i < 100; ++i)
            energy_on_cycles[i] = storage[size - i].right.energy();
    else
        return 7;

    std::sort(energy_on_cycles.begin(), energy_on_cycles.end());

    if (std::abs(energy_on_cycles.back() - energy_on_cycles.front()) /
            energy_on_cycles.back() >
        0.01)
        return 6;

    std::vector<struct peak> peaks = findpeaks(storage.back().right);

    double eps = 1.0;

    if (peaks.size() == 1) return 1;

    if (peaks.size() == 2)
        if (std::abs(peaks[0].power - peaks[1].power) < eps)
            if (std::abs(peaks[0].prominence - peaks[1].prominence) < eps) {
                return 2;
            }
            else
                return 3;

    if (peaks.size() == 3)
        if (std::abs(peaks[0].power - peaks[1].power) < eps &&
            std::abs(peaks[1].power - peaks[2].power) < eps)
            if (std::abs(peaks[0].prominence - peaks[1].prominence) < eps &&
                std::abs(peaks[1].prominence - peaks[2].prominence) < eps) {
                return 4;
                }
            else
                return 5;
    return -peaks.size();
}

void id_response(int id_value, std::ostream& os) {
    switch (id_value) {
        case 1:
            os << "Regime 1: single pulse" << std::endl;
            break;
        case 2:
            os << "Regime 2: 2 spaced pulses" << std::endl;
            break;
        case 3:
            os << "Regime 3: 2 soliton molecule" << std::endl;
            break;
        case 4:
            os << "Regime 4: 3 spaced pulses" << std::endl;
            break;
        case 5:
            os << "Regime 5: 3 soliton molecule" << std::endl;
            break;
        case 6:
            os << "Unstable regimen" << std::endl;
            break;
        case 7:
            os << "Not enough data (< 100 cycles)" << std::endl;
            break;
        default:
            os << "Undefined regimen (" << -id_value << " peaks)" << std::endl;
            break;
    }
}