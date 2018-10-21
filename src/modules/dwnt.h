#ifndef DWNT_H_
#define DWNT_H_

#include "../core/module.h"

// DWNT-SA      -   double-wall nanotubes saturable absorber

// alpha (P_±)  -   normal absorption                               // [%]

// alpha_0      -   modulation depth                                // [%]
// By default   :   36 %

// alpha_ns     -   non-saturable losses                            // [%]
// By default   :   64 %

// P_peak       -   launched peak power                             // [W]

// P_peakSat    -   saturation peak power / saturation power        // [W]
// By default   :   10 W


// P_peakSat i.e. the peak power necessary to reduce
// the absorption coefficient to half the initial value

// alpha (P_±) = alpha_0 / (1 + P_peak / P_sat) + alpha_ns
// sig *= 1 - alpha

class DWNT : public Module {
    double alpha_0, alpha_ns, P_sat;

  public:
    DWNT();
    DWNT(const double& modulation_depth,
         const double& saturation_peak_power,
         const double& non_saturable_losses);
    void setModulationDepth(const double& in_alpha_0);
    void setNonSaturableLosses(const double& in_alpha_ns);
    void setSaturationPeakPower(const double& in_P_sat);

    void execute(Field* signal);
    void execute(Polarizations* signal);
};

#endif  // DWNT_H_