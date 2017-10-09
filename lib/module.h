#ifndef MODULE_H_
#define MODULE_H_

#include <string>
#include "field.h"

class Module {
  public:
    std::string name;

    void setName(std::string name);
    std::string getName();
    virtual void execute(Field& signal) = 0;
    // virtual void execute(std::vector<Field>& signal) = 0;
    virtual void execute(Polarizations& signal) = 0;
};

#endif  // MODULE_H_
