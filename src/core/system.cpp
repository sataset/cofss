#include "system.h"

#include <iostream>

System& System::add(Module* module) {
    modules.push_back(module);

    return *this;
}

System& System::clear() {
    modules.clear();
    return *this;
}

System& System::remove(int module_num) {
    modules.erase(modules.begin() + (module_num - 1));
    return *this;
}

void System::printModules() {
    std::cout << "Modules on execution:" << std::endl;
    for (unsigned long i = 0; i < modules.size() - 1; ++i)
        std::cout << modules[i]->getName() << " -> ";
    std::cout << modules.back()->getName() << std::endl;
}

Field* System::execute(Field* signal) {
    if (modules.empty()) return signal;
    for (unsigned long i = 0; i < modules.size(); ++i)
        modules[i]->execute(signal);

    counter++;
    std::cout << "Cycle " << counter << " - success!" << std::endl;

    return signal;
}

Polarizations* System::execute(Polarizations* signal) {
    if (modules.empty()) return signal;
    for (unsigned long i = 0; i < modules.size(); ++i)
        modules[i]->execute(signal);

    counter++;
    std::cout << "Cycle " << counter << " - success!" << std::endl;

    return signal;
}

unsigned long System::getCount() { return counter; }
void System::resetCount() { counter = 0; }