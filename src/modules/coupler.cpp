#include "coupler.h"

Coupler::Coupler() {
    name = "coupler";
    output_ = nullptr;
    transmission_ = 0.5;
}

Coupler::Coupler(Module* output) {
    name = "coupler";
    output_ = output;
    transmission_ = 0.5;
}

void Coupler::execute(Field* signal) {
    Field output_field(*signal);
    output_field *= sqrt(1.0 - transmission_);
    output_->execute(&output_field);
    
    (*signal) *= sqrt(transmission_);
}

void Coupler::execute(Polarizations* signal) {
    Polarizations output_field(*signal);
    output_field *= sqrt(1.0 - transmission_);
    output_->execute(&output_field);

    (*signal) *= sqrt(1.0 - transmission_);
}

void Coupler::setOuput(Module* module) {
    output_ = module;
}

void Coupler::setTransmission(const double& transmission) {
    transmission_ = transmission;
}