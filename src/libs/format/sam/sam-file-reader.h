#ifndef UTIL_SAM_FILE_READER_H_
#define UTIL_SAM_FILE_READER_H_

#include <list>
#include <string>
#include <istream>
#include "sam-record.h"

namespace format {
    namespace sam {

    class SamFileReader {
        public:
            explicit SamFileReader(std::istream* _in);

            ~SamFileReader();

            size_t readRecords(size_t numRecords, std::list<SamRecord> *records);

        public:
            std::string header;

        private:
            void readHeader();
            std::istream* in;
        };

    }
}  // namespace util

#endif  // UTIL_SAM_FILE_READER_H_
