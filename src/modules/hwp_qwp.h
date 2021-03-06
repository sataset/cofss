#ifndef HWP_QWP_H_
#define HWP_QWP_H_

#include "../core/module.h"

// HWP + QWP    -   half / quarter wave plates

// E_+ = E_1 e^(i psi) cos (chi - pi/4)
// E_- = E_1 e^(i psi) cos (chi + pi/4)

class HWP_QWP : public Module {
    double psi_, chi_;

  public:
    HWP_QWP();
    HWP_QWP(const double& psi, const double& chi);
    void change_psi(double psi);
    void change_chi(double chi);

    void execute(Field* signal);
    void execute(Polarizations* signal);
};

#endif  // HWP_QWP_H_