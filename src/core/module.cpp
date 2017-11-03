#include "module.h"

void Module::setNextModule(Module* nextModule) { next = nextModule; }

void Module::setName(std::string in_name) { name = in_name; }
std::string Module::getName() { return name; }

// void Module::forward(Field* signal) {
//     if (next != nullptr) next->execute(signal);
// }
