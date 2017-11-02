#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include <queue>
#include "field.h"
#include "module.h"

class Executor {
    static Executor* s_instance;
    //std::queue<Field*> field_queue;
    std::queue<Polarizations*> field_queue;
    std::queue<Module*> module_queue;

    Executor();

  public:
    static Executor* instance();
    void execute();
    //void execute(Module* module, Field* field);
    //void enqueue(Module* module, Field* field);
    void execute(Module* module, Polarizations* field);
    void enqueue(Module* module, Polarizations* field);
};

#endif  // EXECUTOR_H_
