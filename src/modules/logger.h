#ifndef LOGGER_H_
#define LOGGER_H_

#include "../core/module.h"
#include "../utility.h"

// Gathering logs from aimed modules

class Logger : public Module {
    std::vector<Polarizations> storage;
    char delimeter_;
    bool delimeter_is_defined;
  public:
    Logger();
    enum WriteType { FREQUENCY, TIME };
    void setDelimeter(char delimeter);
    unsigned long size();
    void clean();
    std::vector<Polarizations>& getStorage();
    Polarizations& get_first();
    Polarizations& get_last();

    void export_metadata(std::ostream& os);
    void export_to_heatmap(std::ostream& os);
    void export_every_to(int n, std::ostream& os, WriteType wt = TIME);
    void export_first_to(std::ostream& os, WriteType wt = TIME);
    void export_last_to(std::ostream& os, WriteType wt = TIME);
    void import_from(std::istream& is);

    void export_derivative_to(std::ostream& os);

    void execute(Field* signal);
    void execute(Polarizations* signal);
};

#endif  // LOGGER_H_