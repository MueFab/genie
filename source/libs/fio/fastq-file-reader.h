#ifndef FIO_FASTQ_FILE_READER_H_
#define FIO_FASTQ_FILE_READER_H_


#include <string>
#include <vector>

#include "fastq-record.h"
#include "file-reader.h"

namespace fio {

    class FastqFileReader : public FileReader {
    public:
        FastqFileReader(const std::string &path);

        ~FastqFileReader();

        size_t readRecords(const size_t numRecords, std::vector<FastqRecord> *const records);
    };

}  // namespace fio

#endif  // FIO_FASTQ_FILE_READER_H_
