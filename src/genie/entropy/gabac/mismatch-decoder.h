/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_MISMATCH_DECODER_H
#define GENIE_GABAC_MISMATCH_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/mismatch-decoder.h>
#include <genie/util/data-block.h>
#include "gabac.h"
#include "decode-transformed-symbols.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
class MismatchDecoder : public core::MismatchDecoder {
   public:
    /**
     *
     * @param d
     */
    explicit MismatchDecoder(util::DataBlock &&d, const EncodingConfiguration &c);

    /**
     * @brief Decodes one base of a mismatch with the knowledge that ref is _not_ the wanted base.
     * @param ref Base to exclude
     * @return One base for a mismatch subsequence
     */
    uint64_t decodeMismatch(uint64_t ref) override;

    /**
     * @brief Tells if there are more mismatches left in the data.
     * @return True if data left.
     */
    bool dataLeft() const override;

    /**
     * @brief Copies the object.
     * @return A copy of the full object state.
     */
    std::unique_ptr<core::MismatchDecoder> copy() const override;

   private:
    uint64_t numSubseqSymbolsTotal;
    uint64_t numSubseqSymbolsDecoded;

    std::vector<TransformedSymbolsDecoder> trnsfSymbolsDecoder;
    size_t numTrnsfSubseqs;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MISMATCH_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------