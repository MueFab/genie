#include "fastq-record.h"

#include <string>
#include <utility>

namespace utils {

    FastqRecord::FastqRecord() : title(""), sequence(""), optional(""), qualityScores("") {}

    FastqRecord::FastqRecord(std::string title, std::string sequence, std::string optional, std::string qualityScores)
            : title(std::move(title)), sequence(std::move(sequence)), optional(std::move(optional)),
              qualityScores(std::move(qualityScores)) {}

    FastqRecord::~FastqRecord() = default;

}  // namespace utils
