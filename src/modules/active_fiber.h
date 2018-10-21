#ifndef ACTIVEFIBER_H_
#define ACTIVEFIBER_H_

#include "../core/module.h"
#include "../utility.h"

// ActiveFiber   - (Thullium)-doped Fiber Amplifier

// g_0    - small signal gain coefficent
// By default : 40dBm

// E_satG - saturation energy
// E_satG = P_satG * T_R
// By default : 4.933412967
// evaluates using defaults below

// P_satG - saturation power of the gain
// By default : 0.03

// T_R    - cavity round trip time
// T_R = L / c_inFiber == ( L n_0 ) / c
// By default : 1.644470989
// (4*0.6e-3 + 1e-3) * 1.45 / 3e-7

// L - fiber length
// By default : 4.0 * 0.6 + 1.0 [m]

// n_0    - refractive index
// By default : 1.45

// g (E_±) = g_0 / (1 + E_± / E_satG)
class ActiveFiber : public Module {
    // AMP parameters
    double satGain, E_satG, T_R, P_satG;

    double alpha;
    double beta2, beta3;
    double gamma;
    double length;
    int total_steps;

    // Filter parameters
    // omega_0 - filter / pulse duration
    double center_wavelength_, omega_0_;

  public:
    ActiveFiber();
    ActiveFiber(const double& small_signal_gain,
         const double& saturation_power,
         const double& cavity_roundtrip_time);

    void setAttenuation(const double& in_alpha);
    void setGain(double (*gain_function)(const Field*));
    void setDispersion(const double& in_beta2);
    void setDispersion(const double& in_beta2, const double& in_beta3);
    void setNonlinearity(const double& in_gamma);
    void setSatGain(const double& small_signal_gain);
    void setSaturationEnergy(const double& saturation_energy);
    void setFiberLength(const double& in_length);
    void setTotalSteps(const int& steps);

    void setCenterWavelength(const double& center_wavelength);
    void setOmega_0(const double& omega_0);

    void execute(Field* signal);
    void execute(Polarizations* signal);

  private:
    Field awgn(const int& samples) const;
    Field filtered_gain(Field* field, const double &step) const;
    Field filtered_gain(Polarizations* field, const double &step) const;
    Field linear_operator(Field* signal, const double &step) const;
    void nonlinear_step(Field* signal, const double &step);
    void nonlinear_step(Polarizations* signal, const double &step);

    Polarizations nonlinearity(Polarizations* signal);

};

#endif  // ACTIVEFIBER_H_
