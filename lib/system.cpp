#include "system.h"

System& System::add(Module *module) {
	modules.push_back(module);

	return *this;
}

System& System::clear(bool remove_modules) {
	if (remove_modules)
		modules.clear();
	if (storage.empty())
		storage.clear();
	
	return *this;
}

System& System::remove(int module_num) {
	modules.erase(modules.begin()+(module_num - 1));
	clear(false);
	return *this;
}

Field System::execute(Field& signal) {
	clear(false);

	if (modules.empty()) return signal;

	storage.push_back(signal);
	for(int i = 0; i < modules.size(); i++)
	{
		modules[i]->execute(signal);
		storage.push_back(signal);
	}

	return signal;
}

Field System::current_state() {
	return storage.back();
}