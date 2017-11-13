#include "amplifier_simple.h"

Amplifier_simple::Amplifier_simple() { gain_ = 1.0; }

Amplifier_simple::Amplifier_simple(const double& gain) { gain_ = gain; }

void Amplifier_simple::setGain(const double& gain) { gain_ = gain; }

void Amplifier_simple::execute(Field* signal) { *signal *= sqrt(gain_); }

void Amplifier_simple::execute(Polarizations* signal) {
    *signal *= sqrt(gain_);
}