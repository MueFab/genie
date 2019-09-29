#ifndef UTIL_FASTQ_RECORD_H_
#define UTIL_FASTQ_RECORD_H_

#include <string>

namespace util {

struct FastqRecord {
   public:
    FastqRecord();
    FastqRecord(std::string title, std::string sequence, std::string optional, std::string qualityScores);
    ~FastqRecord();

   public:
    std::string title;
    std::string sequence;
    std::string optional;
    std::string qualityScores;
};

}  // namespace util

#endif  // UTIL_FASTQ_RECORD_H_
