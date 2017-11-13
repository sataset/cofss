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
    unsigned long size;
    size = storage.back().right.size();
    os.precision(15);
    for (unsigned long i = 0; i < storage.size(); ++i) {
        for (unsigned long j = 0; j < size; ++j)
            os << storage[i].right.t(j) << ','
            << storage[i].right[j].real() << ','
            << storage[i].right[j].imag() << ','
            << storage[i].left[j].real() << ','
            << storage[i].left[j].imag() << std::endl;
        os << '\n' << std::endl;
    }
}

void Logger::getFirstNLast(std::ostream& os) {
    unsigned long size;
    size = storage.back().right.size();
    os.precision(15);
    for (unsigned long i = 0; i < size; ++i)
        os << storage.front().right.t(i) << ','
        << storage.front().right[i].real() << ','
        << storage.front().right[i].imag() << ','
        << storage.front().left[i].real() << ','
        << storage.front().left[i].imag() << std::endl;
    os << '\n' << std::endl;

    for (unsigned long i = 0; i < size; ++i)
        os << storage.back().right.t(i) << ','
        << storage.back().right[i].real() << ','
        << storage.back().right[i].imag() << ','
        << storage.back().left[i].real() << ','
        << storage.back().left[i].imag() << std::endl;
    os << '\n' << std::endl;
}

void Logger::getCurrentState(std::ostream& os) {
    unsigned long size;
    size = storage.back().right.size();
    os.precision(15);
      for (unsigned long i = 0; i < size; ++i)
        os << storage.back().right.t(i) << ','
        << storage.back().right[i].real() << ','
        << storage.back().right[i].imag() << ','
        << storage.back().left[i].real() << ','
        << storage.back().left[i].imag() << std::endl;
    os << '\n' << std::endl;
}

void Logger::execute(Field* signal) {
    storage.push_back(Polarizations{*signal, Field(signal->size(), 0)});
}

void Logger::execute(Polarizations* signal) { storage.push_back(*signal); }