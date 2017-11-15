#include "filter.h"

Filter::Filter() {
    name = "freq_filter";
    envelope_curve = new Field(1,1);
}

Filter::Filter(const Field* field) {
    name = "freq_filter";
    for(unsigned long i = 0; i < field->size(); ++i)
        envelope_curve[i] = field[i];
}

void Filter::execute(Field* signal) {
    signal->fft_inplace();
    (*signal) *= sqrt(*envelope_curve);
    signal->ifft_inplace();
}

void Filter::execute(Polarizations* signal) {
    signal->fft_inplace();
    (*signal) *= sqrt(*envelope_curve);
    signal->ifft_inplace();

    // Executor::instance()->enqueue(next, signal);
}