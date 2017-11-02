#include "executor.h"

Executor* Executor::s_instance = nullptr;

Executor::Executor(){};

Executor* Executor::instance() {
    if (!s_instance) s_instance = new Executor;
    return s_instance;
}

void Executor::execute() {
    while (!module_queue.empty()) {
        module_queue.front()->execute(field_queue.front());
        module_queue.pop();
        field_queue.pop();
    }
}

// void Executor::execute(Module* module, Field* field) {
//     enqueue(module, field);
//     execute();
// }

// void Executor::enqueue(Module* module, Field* field) {
//     field_queue.push(field);
//     module_queue.push(module);
// }

void Executor::execute(Module* module, Polarizations* field) {
    enqueue(module, field);
    execute();
}

void Executor::enqueue(Module* module, Polarizations* field) {
    module_queue.push(module);
    field_queue.push(field);
}
