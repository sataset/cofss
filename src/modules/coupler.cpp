#include "coupler.h"

Coupler::Coupler() {
    name = "coupler";
    output_ = nullptr;
    transmission = 0.6;
}

Coupler::Coupler(Module* output) {
    name = "coupler";
    output_ = output;
    transmission = 0.6;
}

// void Coupler::execute(Field* signal) {
//     Executor::instance()->enqueue(output, new Field((*signal)*0.6));
//     (*signal) *= 0.4;
//
//     Executor::instance()->enqueue(next, signal);
// }

// void Coupler::execute(Polarizations* signal) {
//     if (output != nullptr)
//         Executor::instance()->enqueue(output,
//                                       new Polarizations((*signal) * 0.6));
//     (*signal) *= 0.4;

//     Executor::instance()->enqueue(next, signal);
// }

void Coupler::execute(Field* signal) {
    output_->execute(new Field((*signal) * transmission));
    (*signal) *= sqrt(1.0 - transmission);
}

void Coupler::execute(Polarizations* signal) {
    output_->execute(new Polarizations((*signal) * transmission));
    (*signal) *= sqrt(1.0 - transmission);
}

void Coupler::setOuput(Module* module) {
    output_ = module;
}

void Coupler::setTransmission(const double& percentage) {
    transmission = percentage / 100.0;
}