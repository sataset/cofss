#include "amplifier.h"

Amplifier::Amplifier() {
    name = "amp";
    gain = 1.;
}

Amplifier::Amplifier(const double& gain_value) {
    name = "amp";
    gain = gain_value;
}

void Amplifier::execute(Field& signal) {
    signal *= sqrt(gain);
}

void Amplifier::execute(Polarizations& signal) {
    signal *= sqrt(gain);
}