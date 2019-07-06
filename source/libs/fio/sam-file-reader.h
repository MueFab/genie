#ifndef FIO_SAM_FILE_READER_H_
#define FIO_SAM_FILE_READER_H_

#include <string>
#include <vector>

#include "file-reader.h"
#include "sam-record.h"

namespace fio {

    class SamFileReader : public FileReader {
    public:
        SamFileReader(const std::string &path);

        ~SamFileReader();

        size_t readRecords(const size_t numRecords, std::vector<SamRecord> *const records);

    public:
        std::string header;

    private:
        void readHeader(void);
    };


}  // namespace fio

#endif  // FIO_SAM_FILE_READER_H_
