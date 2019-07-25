#include "fastq-record.h"

#include <string>


namespace genie {


FastqRecord::FastqRecord() : title(""), sequence(""), optional(""), qualityScores("") {}


FastqRecord::FastqRecord(
    const std::string& title,
    const std::string& sequence,
    const std::string& optional,
    const std::string& qualityScores)
    : title(title),
      sequence(sequence),
      optional(optional),
      qualityScores(qualityScores)
{
    // Nothing to do here.
}


FastqRecord::~FastqRecord() = default;


}  // namespace genie
