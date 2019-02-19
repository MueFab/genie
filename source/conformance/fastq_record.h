#ifndef GENIE_FASTQRECORD_H_
#define GENIE_FASTQRECORD_H_


#include <string>


namespace genie {


struct FastqRecord {
 public:
    FastqRecord(void);

    FastqRecord(
        const std::string& title,
        const std::string& sequence,
        const std::string& optional,
        const std::string& qualityScores);

    ~FastqRecord(void);

 public:
    std::string title;
    std::string sequence;
    std::string optional;
    std::string qualityScores;
};


}  // namespace genie


#endif  // GENIE_FASTQRECORD_H_

