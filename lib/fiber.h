#ifndef FIBER_H_
#define FIBER_H_

#include "field.h"
#include "module.h"
#include "utility.h"

class Fiber : public Module {
    double alpha;
    double beta2, beta3;
    double gamma;
    double fiber_length;
    double wavelength;
    double sampling_rate;  // Move to field
    int total_steps;

  public:
    Fiber();
    Fiber(const double& center_wavelength);
    void setAttenuation(const double& alpha);
    void setAttenuationDB(const double& alpha_dB);
    void setDispersion(const double& beta2);
    void setDispersion(const double& beta2, const double& beta3);
    void setNonlinearity(const double& gamma);
    void setFiberLength(const double& length);
    void setTotalSteps(const int& steps);
    void setSamplingRate(const double& rate);

    void execute(Field& signal);
    void execute(Polarizations& signal);
    // void execute(std::vector<Field>& signal);

  private:
    Field estimateLinearity(const int& samples) const;
};

#endif  // FIBER_H_
