#ifndef FIO_FASTA_RECORD_H_
#define FIO_FASTA_RECORD_H_

#include <string>

namespace utils {

    struct FastaRecord {
    public:
        FastaRecord(std::string header, std::string sequence);

        ~FastaRecord();

    public:
        std::string header;
        std::string sequence;
    };

}  // namespace utils

#endif  // FIO_FASTA_RECORD_H_
