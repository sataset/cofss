#include <fstream>
#include <iostream>
#include <string>

#include "lib/system.h"
#include "lib/fiber.h"
#include "lib/field.h"
#include "lib/utility.h"

// reference units [m], [s], [W]
const double distance = 1e5;         // [m]
const double attenuation = 2e-4;     // [dB/m]
const double dispersion = 1.7e-5;    // [s/m/m]
const double wavelength = 1.55e-6;   // [m]
const double nonlinearity = 1.4e-3;  // [1/W/m]
const int pulse_width = 1024;        // [1]


Field& sech_pulse(const int& nodes_quantity, const double& width) {
    Field sech_pulse(nodes_quantity, 0);
    double argument;
    for (int i = 0; i < nodes_quantity; i++) {
        argument = double(i) / (nodes_quantity - 1.0);
        argument = 20 * argument - 20 / 2.0;
        sech_pulse[i] = 1.0 / std::cosh(argument / width);
        sech_pulse[i] /= width;
    }

    return sech_pulse;
}

int main() {
    System main_sys;
    main_sys.add(new Fiber(wavelength));

    main_sys.execute(sech_pulse(16, 10));

    // ComplexVector cur_st = main_sys.current_state();

    //for(int i = 0; i < cur_st.size(); i++)
    //	std::cout << cur_st[i] << std::endl;  

    return 0;
}
