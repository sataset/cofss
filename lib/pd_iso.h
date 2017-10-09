#ifndef PD_ISO_
#define PD_ISO_

#include "field.h"
#include "module.h"
#include "utility.h"

class PD_ISO : public Module {

  public:
    PD_ISO();

    void execute(Field& signal);
    void execute(Polarizations& signal);
    // void execute(std::vector<Field>& signal);
};

#endif  // PD_ISO_
