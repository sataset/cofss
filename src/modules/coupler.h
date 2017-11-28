#ifndef COUPLER_H_
#define COUPLER_H_

#include "../core/module.h"

// Separates signal. 
// First part continues propogation. 
// Second goes to container.

class Coupler : public Module {
    Module* output_;
    double transmission_;
  public:
    Coupler();
    Coupler(Module* output);

    void execute(Field* signal);
    void execute(Polarizations* signal);
    void setOuput(Module* module);
    void setTransmission(const double& transmission);
};

#endif  // COUPLER_H_