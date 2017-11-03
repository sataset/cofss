#ifndef FIBER_H_
#define FIBER_H_

#include "../core/module.h"
#include "../utility.h"

class Fiber : public Module {
    double alpha;
    // double (*gain)(const Field*);
    double beta2, beta3;
    double gamma;
    double length;
    int total_steps;

  public:
    Fiber();
    void setAttenuation(const double& in_alpha);
    // void setGain(double (*gain_function)(const Field*));
    void setDispersion(const double& in_beta2);
    void setDispersion(const double& in_beta2, const double& in_beta3);
    void setNonlinearity(const double& in_gamma);
    void setFiberLength(const double& in_length);
    void setTotalSteps(const int& steps);

    void execute(Field* signal);
    void execute(Polarizations* signal);

  private:
    Field estimateLinearity(Field* signal) const;
    Field estimateLinearity(Field& signal) const;
};

#endif  // FIBER_H_
