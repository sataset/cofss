#ifndef MODULE_H_
#define MODULE_H_

#include <string>
//#include "executor.h"
#include "field.h"

class Module {
  public:
    std::string name;
    Module* next;

    void setName(std::string name);
    std::string getName();
    void setNextModule(Module* nextModule);

    virtual void execute(Field* signal) = 0;
    virtual void execute(Polarizations* signal) = 0;

  private:
    //void forward(Field* signal);
};

#endif  // MODULE_H_
