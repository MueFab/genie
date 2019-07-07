#ifndef FIO_SAM_FILE_READER_H_
#define FIO_SAM_FILE_READER_H_

#include <list>
#include <string>

#include "file-reader.h"
#include "sam-record.h"

namespace fio {

    class SamFileReader : public FileReader {
    public:
        explicit SamFileReader(const std::string &path);

        ~SamFileReader() override;

        size_t readRecords(size_t numRecords, std::list<SamRecord> *records);

    public:
        std::string header;

    private:
        void readHeader();
    };


}  // namespace fio

#endif  // FIO_SAM_FILE_READER_H_
