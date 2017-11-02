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
    System(){ stop_flag = 0; counter = 0; };

    System& add(Module* module);
    System& clear(bool remove_modules);
    System& remove(int module_num);
    void printModules();
    Field* execute(Field* signal);
	Polarizations* execute(Polarizations* signal);
	//std::vector<Field> execute(std::vector<Field>& signal);

    unsigned long getCount();
    void resetCount();
	// Field current_state();
};

#endif  // SYSTEM_H
