#ifndef GENIE_FASTQ_RECORD_H_
#define GENIE_FASTQ_RECORD_H_

#include <string>

namespace genie {

struct FastqRecord {
   public:
    FastqRecord();
    FastqRecord(const std::string& title, const std::string& sequence, const std::string& optional,
                const std::string& qualityScores);
    ~FastqRecord();

   public:
    std::string title;
    std::string sequence;
    std::string optional;
    std::string qualityScores;
};

}  // namespace genie

#endif  // GENIE_FASTQ_RECORD_H_
