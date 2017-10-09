#include "pd_iso.h"

PD_ISO::PD_ISO() { name = "pd_iso"; }

void PD_ISO::execute(Field& signal) {}

void PD_ISO::execute(Polarizations& signal) {
    signal.y = Field(signal.y.size(), 0);
}