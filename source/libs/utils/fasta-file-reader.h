#ifndef FIO_FASTA_FILE_READER_H_
#define FIO_FASTA_FILE_READER_H_

#include <string>
#include <vector>

#include "fasta-record.h"
#include "file-reader.h"

namespace utils {

class FastaFileReader : public FileReader {
   public:
    FastaFileReader(const std::string &path);

    ~FastaFileReader();

    void parse(std::vector<FastaRecord> *const fastaRecords);
};

}  // namespace utils

#endif  // FIO_FASTA_FILE_READER_H_
