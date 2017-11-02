#include "hwp_qwp.h"
#include "../utility.h"

HWP_QWP::HWP_QWP() {
    name = "quarter_wp";
    psy = 0.056;
    xi = 0.1;
}

HWP_QWP::HWP_QWP(const double& psy, const double& xi) {
    this->psy = psy;
    this->xi = xi;
}

void HWP_QWP::execute(Field* signal) {}

void HWP_QWP::execute(Polarizations* signal) {
    Field E_1 = signal->x();
    signal->right = E_1 * i_exp(psy) * std::cos(xi - M_PI / 4.0);
    signal->left = E_1 * i_exp(psy) * std::cos(xi + M_PI / 4.0);

    //Executor::instance()->enqueue(next, signal);
}