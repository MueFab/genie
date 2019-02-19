#include "conformance/fastq_record.h"

#include <string>


namespace genie {


FastqRecord::FastqRecord(void)
    : title(""),
      sequence(""),
      optional(""),
      qualityScores("")
{
    // Nothing to do here.
}


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


FastqRecord::~FastqRecord(void)
{
    // Nothing to do here.
}


}  // namespace genie
