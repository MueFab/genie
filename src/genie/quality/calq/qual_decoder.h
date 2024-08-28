/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_QUAL_DECODER_H_
#define SRC_GENIE_QUALITY_CALQ_QUAL_DECODER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <utility>

// -----------------------------------------------------------------------------

#include "genie/quality/calq/quantizer.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

struct DecodingRead;
struct EncodingBlock;
struct DecodingBlock;

// -----------------------------------------------------------------------------

struct DecodingRead {
    uint64_t posMin;
    std::string cigar;
};

// -----------------------------------------------------------------------------

class QualDecoder {
 public:
    QualDecoder(const DecodingBlock& in, uint64_t positionOffset, uint8_t qualityOffset, EncodingBlock* out);
    ~QualDecoder();

    void decodeMappedRecordFromBlock(const DecodingRead& samRecord);

 private:
    uint64_t posOffset_;
    int qualityValueOffset_;

    std::vector<size_t> qviIdx_;

    std::vector<Quantizer> quantizers_;

    EncodingBlock* out;
    const DecodingBlock& in;
};

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_QUAL_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
