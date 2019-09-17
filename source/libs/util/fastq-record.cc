#include "fastq-record.h"
#include <string>
#include <utility>

namespace util {

FastqRecord::FastqRecord() : title(""), sequence(""), optional(""), qualityScores("") {}

FastqRecord::FastqRecord(std::string ptitle, std::string psequence, std::string poptional, std::string pqualityScores)
    : title(std::move(ptitle)),
      sequence(std::move(psequence)),
      optional(std::move(poptional)),
      qualityScores(std::move(pqualityScores)) {}

FastqRecord::~FastqRecord() = default;

}  // namespace util
