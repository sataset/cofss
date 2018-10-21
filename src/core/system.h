#ifndef SYSTEM_H
#define SYSTEM_H

#include <ostream>
#include "field.h"
#include "module.h"

class System {

    std::vector<Module*> modules;
    unsigned long counter;
    bool stop_flag;

  public:
    System();
    System& add(Module* module);
    System& clear();
    System& remove(int module_num);
    void printModules();
    Field* execute(Field* signal);
	Polarizations* execute(Polarizations* signal);

    unsigned long getCount();
    void resetCount();
};

#endif  // SYSTEM_H
