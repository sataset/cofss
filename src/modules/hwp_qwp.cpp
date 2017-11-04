#include "hwp_qwp.h"
#include "../utility.h"

HWP_QWP::HWP_QWP() {
    name = "hwp+qwp";
    psi = 0.056;
    xi = 0.1;
}

HWP_QWP::HWP_QWP(const double& in_psi, const double& in_xi) {
    name = "hwp+qwp";
    psi = in_psi;
    xi = in_xi;
}

void HWP_QWP::execute(Field* signal) {}

void HWP_QWP::execute(Polarizations* signal) {
    Field E_1 = signal->x();
    signal->right = E_1 * i_exp(psi) * std::cos(xi - math_pi / 4.0);
    signal->left = E_1 * i_exp(-psi) * std::cos(xi + math_pi / 4.0);

    //Executor::instance()->enqueue(next, signal);
}