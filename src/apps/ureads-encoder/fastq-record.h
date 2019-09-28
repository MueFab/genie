#ifndef GENIE_FASTQ_RECORD_H_
#define GENIE_FASTQ_RECORD_H_

#include <string>

namespace genie {

struct FastqRecord {
   public:
    FastqRecord();
    FastqRecord(const std::string& title_, const std::string& sequence_, const std::string& optional_,
                const std::string& qualityScores_);
    ~FastqRecord();

   public:
    std::string title;
    std::string sequence;
    std::string optional;
    std::string qualityScores;
};

}  // namespace genie

#endif  // GENIE_FASTQ_RECORD_H_
