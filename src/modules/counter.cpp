#include "counter.h"

Counter::Counter() {
    name = "counter";
    counter = 0;
}

int Counter::getCount() { return counter; }

void Counter::execute(Field* signal) {
    counter++;
    // Executor::instance()->enqueue(next, signal);
}

void Counter::execute(Polarizations* signal) {
    counter++;
    // Executor::instance()->enqueue(next, signal);
}