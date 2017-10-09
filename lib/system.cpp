#include "system.h"

#include <iostream>

System& System::add(Module* module) {
    modules.push_back(module);

    return *this;
}

System& System::clear(bool remove_modules = 0) {
    modules.clear();
    return *this;
}

System& System::remove(int module_num) {
    modules.erase(modules.begin() + (module_num - 1));
    return *this;
}

Field System::execute(Field& signal) {
    if (modules.empty()) return signal;

    for (unsigned long i = 0; i < modules.size(); ++i)
        modules[i]->execute(signal);

    return signal;
}

// std::vector<Field> System::execute(std::vector<Field>& signal) {
//     if (modules.empty()) return signal;

//     for (unsigned long i = 0; i < modules.size(); ++i)
//         modules[i]->execute(signal);

//     return signal;
// }

Polarizations System::execute(Polarizations& signal)  {
    if (modules.empty()) return signal;

    for (unsigned long i = 0; i < modules.size(); ++i)
        modules[i]->execute(signal);

    return signal;
}

// Field System::current_state() {
//     // temp_debuger();
//     return cur_signal;
// }

// void System::output_state(std::ostream& os) const {
//     for (unsigned long i = 0; i < cur_signal.size(); i++)
//         os << cur_signal[i].real() << '\t' << cur_signal[i].imag()
//            << '\n';
//     os << std::flush;
// }