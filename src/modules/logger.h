#ifndef LOGGER_H_
#define LOGGER_H_

#include "../core/module.h"
#include "../utility.h"

// Gathering logs from aimed modules

class Logger : public Module {
    std::vector<Polarizations> storage;
  public:
    Logger();
    enum WriteType { FREQUENCY, TIME };
    //void getLogs(std::ostream& os, bool for_field);

    void write_logs_to(std::ostream& os, WriteType wt = TIME);
    void write_first_to(std::ostream& os, WriteType wt = TIME);
    void write_last_to(std::ostream& os, WriteType wt = TIME);

    void execute(Field* signal);
    void execute(Polarizations* signal);
};

#endif  // LOGGER_H_