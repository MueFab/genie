/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_MISMATCH_DECODER_H_
#define SRC_GENIE_ENTROPY_GABAC_MISMATCH_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/core/mismatch-decoder.h"
#include "genie/entropy/gabac/decode-transformed-symbols.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
class MismatchDecoder : public core::MismatchDecoder {
 public:
    /**
     * @brief
     * @param d
     * @param c
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
    [[nodiscard]] bool dataLeft() const override;

    /**
     * @brief get the total number of symbols in the subsequence.
     * @return number of symbols.
     */
    [[nodiscard]] uint64_t getSubseqSymbolsTotal() const;

    /**
     * @brief Copies the object.
     * @return A copy of the full object state.
     */
    [[nodiscard]] std::unique_ptr<core::MismatchDecoder> copy() const override;

    /**
     * @brief
     */
    std::vector<util::DataBlock> trnsfSubseqData;

 private:
    uint64_t numSubseqSymbolsTotal;    //!< @brief
    uint64_t numSubseqSymbolsDecoded;  //!< @brief

    std::vector<TransformedSymbolsDecoder> trnsfSymbolsDecoder;  //!< @brief
    size_t numTrnsfSubseqs;                                      //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_MISMATCH_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
