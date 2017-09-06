#ifndef SYSTEM_H
#define SYSTEM_H

#include "field.h"
#include "module.h"

class System {
	
	std::vector<Field> storage;
	std::vector<Module*> modules;

public:

	System(){};

	System& add(Module* module);
	System& clear(bool remove_modules);
	System& remove(int module_num);
	Field execute(Field& signal);

	Field current_state();

};

#endif // SYSTEM_H