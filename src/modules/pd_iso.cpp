#include "pd_iso.h"

PD_ISO::PD_ISO() { name = "pd_iso"; }

void PD_ISO::execute(Field* signal) {}

void PD_ISO::execute(Polarizations* signal) {
    signal->right = (signal->right + signal->left) / 2.0;
    signal->left = signal->right;

    // Executor::instance()->enqueue(next, signal);
}