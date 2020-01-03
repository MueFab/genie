/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_REFERENCE_ENCODER_H
#define GENIE_REFERENCE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_rec/mpegg-record.h>
#include <limits>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace lae {

class LocalAssemblyReferenceEncoder {
   public:
    uint32_t cr_buf_max_size;

    std::vector<std::string> sequences;
    std::vector<uint64_t> sequence_positions;
    uint32_t crBufSize;

    std::string generateRef(uint32_t offset, uint32_t len);

    char majorityVote(uint32_t offset_to_first);

    static std::string preprocess(const std::string &read, const std::string &cigar);

    static uint32_t lengthFromCigar(const std::string &cigar);

   public:
    explicit LocalAssemblyReferenceEncoder(uint32_t _cr_buf_max_size);

    void addSingleRead(const std::string &record, const std::string &ecigar, uint64_t position);

    void addRead(const format::mpegg_rec::MpeggRecord &s);

    std::string getReference(uint32_t pos_offset, const std::string &cigar);

    std::string getReference(uint32_t pos_offset, uint32_t len);

    uint64_t getWindowBorder() const;

    void printWindow() const;
};
}  // namespace lae

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_REFERENCE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------