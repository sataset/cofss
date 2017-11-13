#ifndef AMPLIFIER_SIMPLE_
#define AMPLIFIER_SIMPLE_

#include "../core/module.h"
#include "../utility.h"

// Simple amp without noise

class Amplifier_simple : public Module {
    double gain_;
  public:
    Amplifier_simple();
    Amplifier_simple(const double& gain);
    
    void setGain(const double& gain);

    void execute(Field* signal);
    void execute(Polarizations* signal);
    // void execute(std::vector<Field>& signal);
};

#endif  // AMPLIFIER_SIMPLE_