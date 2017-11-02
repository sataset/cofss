#ifndef COUNTER_H_
#define COUNTER_H_

#include "../core/module.h"

// Just counter

class Counter : public Module {
    int counter;

  public:
    Counter();

    int getCount();

    void execute(Field* signal);
    void execute(Polarizations* signal);
};

#endif  // COUNTER_H_