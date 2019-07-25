#include <utility>

#include "fasta-record.h"

#include <iostream>

namespace utils {

    FastaRecord::FastaRecord(std::string head, std::string seq)
            : header(std::move(head)),
              sequence(std::move(seq)) {}

    FastaRecord::~FastaRecord() = default;

}  // namespace utils
