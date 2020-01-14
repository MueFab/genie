#include <utility>

#include <iostream>
#include "fasta-record.h"

namespace genie {
namespace fasta {

FastaRecord::FastaRecord(std::string head, std::string seq) : header(std::move(head)), sequence(std::move(seq)) {}

FastaRecord::~FastaRecord() = default;

}  // namespace fasta
}  // namespace genie
