#ifndef LOGGER_H_
#define LOGGER_H_

#include "../core/module.h"

// Gathering logs from aimed modules

class Logger : public Module {
    std::vector<Polarizations> storage;
  public:
    Logger();

    //void getLogs(std::ostream& os, bool for_field);
    void getLogs(std::ostream& os);
    void getFirstNLast(std::ostream& os);
    void getCurrentState(std::ostream& os);

    void execute(Field* signal);
    void execute(Polarizations* signal);
};

#endif  // LOGGER_H_