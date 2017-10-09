#ifndef SYSTEM_H
#define SYSTEM_H

#include <ostream>
#include "field.h"
#include "module.h"

class System {

    std::vector<Module*> modules;

  public:
    System(){};

    System& add(Module* module);
    System& clear(bool remove_modules);
    System& remove(int module_num);
    Field execute(Field& signal);
	//std::vector<Field> execute(std::vector<Field>& signal);
	Polarizations execute(Polarizations& signal);

	// Field current_state();
    // void output_state(std::ostream& os) const;
};

#endif  // SYSTEM_H
