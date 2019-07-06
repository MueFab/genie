#ifndef FIO_FASTQ_RECORD_H_
#define FIO_FASTQ_RECORD_H_

#include <string>

namespace fio {

struct FastqRecord {
 public:
    FastqRecord();

    FastqRecord(
        std::string  title,
        std::string  sequence,
        std::string  optional,
        std::string  qualityScores);

    ~FastqRecord(void);

 public:
    std::string title;
    std::string sequence;
    std::string optional;
    std::string qualityScores;
};

}  // namespace fastq

#endif  // FIO_FASTQ_RECORD_H_

