#ifndef UTIL_FASTA_FILE_READER_H_
#define UTIL_FASTA_FILE_READER_H_

#include <string>
#include <vector>
#include "fasta-record.h"
#include "file-reader.h"

namespace util {

class FastaFileReader : public FileReader {
   public:
    FastaFileReader(const std::string &path);

    ~FastaFileReader();

    void parse(std::vector<FastaRecord> *const fastaRecords);
};

}  // namespace util

#endif  // UTIL_FASTA_FILE_READER_H_
