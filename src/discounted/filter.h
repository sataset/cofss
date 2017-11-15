#ifndef FILTER_H_
#define FILTER_H_

#include "../core/module.h"
#include "../utility.h"

// Lorentzian line shape filter

class Filter : public Module {
    Field* envelope_curve;

  public:
    Filter();
    Filter(const Field* field);

    void execute(Field* signal);
    void execute(Polarizations* signal);
};

#endif  // FILTER_H_