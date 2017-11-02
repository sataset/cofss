#include "logger.h"

Logger::Logger() { name = "logger"; }

// void Logger::getLogs(std::ostream& os, bool for_field) {
//     unsigned long size = 0;
//     if (for_field) {
//         size = field_storage.back().size();
//         while (!field_storage.empty()) {
//             for (unsigned long i = 0; i < size; ++i)
//                 os << field_storage.back().w(i) << '\t'
//                    << norm(field_storage.back()[i]) << std::endl;
//             os << '\n' << std::endl;
//         }
//     } else {
//         size = storage.back().right.size();
//         while (!storage.empty()) {
//             for (unsigned long i = 0; i < size; ++i)
//                 os << storage.back().right.w(i) << '\t'
//                    << norm(storage.back().right[i])
//                    << storage.back().left.w(i) << '\t'
//                    << norm(storage.back().left[i]) <<
//                    std::endl;
//             os << '\n' << std::endl;
//         }
//     }
// }

void Logger::getLogs(std::ostream& os) {
    unsigned long size = 0;
    size = storage.back().right.size();
    for (unsigned long i = 0; i < storage.size(); ++i) {
        for (unsigned long j = 0; j < size; ++j)
            os << sqrt(norm(storage[i].right[j])) << ",\t"
               << sqrt(norm(storage[i].left[j])) << std::endl;
        os << '\n' << std::endl;
    }
}

void Logger::getFirstNLast(std::ostream& os) {
    unsigned long size = 0;
    size = storage.back().right.size();

    for (unsigned long i = 0; i < size; ++i)
        os << sqrt(norm(storage.front().right[i])) << ",\t"
           << sqrt(norm(storage.front().left[i])) << std::endl;
    os << '\n' << std::endl;

    for (unsigned long i = 0; i < size; ++i)
        os << sqrt(norm(storage.back().right[i])) << ",\t"
           << sqrt(norm(storage.back().left[i])) << std::endl;
    os << '\n' << std::endl;
}

void Logger::execute(Field* signal) {
    storage.push_back(
        Polarizations{*signal, Field(signal->size(), 0)});
}

void Logger::execute(Polarizations* signal) {
    storage.push_back(*signal);
}