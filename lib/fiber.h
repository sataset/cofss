#ifndef FIBER_H_
#define FIBER_H_

#include "module.h"
#include "field.h"
const double planck = 6.62607004081e-34;
const double light_speed = 299792458;

class Fiber : public Module {
    double alpha;
    double beta2;
    double gamma;
    double fiber_length;
    double wavelength;
    double sampling_rate;
    int total_steps;

  public:
    Fiber();
    Fiber(const double& center_wavelength);
    void setAttenuation(const double& alpha);
    void setAttenuationDB(const double& alpha_dB);
    void setDispersionPhysical(const double& dispersion);
    void setDispersionEngineering(const double& beta2);
    void setNonlinearity(const double& gamma);
    void setFiberLength(const double& length);
    void setTotalSteps(const int& steps);
    void setSamplingRate(const double& rate);

    void execute(Field& signal);

  private:
    Field estimateLinearity(const int& samples) const;
};

#endif  // FIBER_H_
