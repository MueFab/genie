/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "record.h"

namespace genie {
namespace format {
namespace fasta {

FastaRecord::FastaRecord(std::string head, std::string seq) : header(std::move(head)), sequence(std::move(seq)) {}

FastaRecord::~FastaRecord() = default;

}  // namespace fasta
}  // namespace format
}  // namespace genie
