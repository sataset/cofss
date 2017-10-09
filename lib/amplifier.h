#ifndef AMPLIFIER_H_
#define AMPLIFIER_H_

#include "field.h"
#include "module.h"
#include "utility.h"

class Amplifier : public Module {
    double gain;

  public:
    Amplifier();
    Amplifier(const double& gain_value);

    void execute(Field& signal);
    void execute(Polarizations& signal);
    // void execute(std::vector<Field>& signal);
};

#endif  // AMPLIFIER_H_
