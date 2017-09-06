#ifndef MODULE_H
#define MODULE_H

#include <string>
#include "field.h"

class Module {
public:
	std::string name;
	
	void setName(std::string name);
	std::string getName();
	virtual void execute(Field& signal) = 0;
};

#endif // MODULE_H