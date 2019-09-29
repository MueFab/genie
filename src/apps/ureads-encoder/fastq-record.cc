#include "fastq-record.h"

#include <string>

namespace genie {

FastqRecord::FastqRecord() : title(""), sequence(""), optional(""), qualityScores("") {}

FastqRecord::FastqRecord(const std::string& title_, const std::string& sequence_, const std::string& optional_,
                         const std::string& qualityScores_)
    : title(title_), sequence(sequence_), optional(optional_), qualityScores(qualityScores_) {
    // Nothing to do here.
}

FastqRecord::~FastqRecord() = default;

}  // namespace genie
