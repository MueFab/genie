#ifndef UTILS_FASTQ_RECORD_H_
#define UTILS_FASTQ_RECORD_H_

#include <string>

namespace utils {

struct FastqRecord {
 public:
    FastqRecord();

    FastqRecord(std::string  title,std::string  sequence,std::string  optional,std::string  qualityScores);

    ~FastqRecord();

 public:
    std::string title;
    std::string sequence;
    std::string optional;
    std::string qualityScores;
};

}  // namespace utils

#endif  // UTILS_FASTQ_RECORD_H_

