#ifndef PD_ISO_
#define PD_ISO_

#include "../core/module.h"
#include "../utility.h"

// PD-ISO / PBM - Polarization Dependent Isolator / Polarization Beam Splitter

class PD_ISO : public Module {

  public:
    PD_ISO();

    void execute(Field* signal);
    void execute(Polarizations* signal);
    // void execute(std::vector<Field>& signal);
};

#endif  // PD_ISO_
