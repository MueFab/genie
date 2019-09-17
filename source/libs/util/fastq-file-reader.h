#ifndef UTIL_FASTQ_FILE_READER_H_
#define UTIL_FASTQ_FILE_READER_H_

#include <string>
#include <vector>
#include "fastq-record.h"
#include "file-reader.h"

namespace util {

class FastqFileReader : public FileReader {
   public:
    FastqFileReader(const std::string &path);
    ~FastqFileReader();
    size_t readRecords(const size_t numRecords, std::vector<FastqRecord> *const records);
};

}  // namespace util

#endif  // UTIL_FASTQ_FILE_READER_H_
