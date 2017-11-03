#ifndef MODULATIONS_H_
#define MODULATIONS_H_

#include "core/field.h"
#include "utility.h"

Field sech_pulse(const int& nodes_quantity, const double& width);
Field gaussian(const int& nodes_quantity,
               const double& peak_power,
               const double& time_width,
               const double& total_width);
#endif  // MODULATIONS_H_