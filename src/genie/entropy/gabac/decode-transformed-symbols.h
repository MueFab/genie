/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/entropy/gabac/context-selector.h"
#include "genie/entropy/gabac/decode-transformed-subseq.h"
#include "genie/entropy/gabac/luts-subsymbol-transform.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
class TransformedSymbolsDecoder {
 public:
    /**
     *
     * @param
     */
    explicit TransformedSymbolsDecoder(util::DataBlock *bitstream, const paramcabac::TransformedSubSeq &trnsfSubseqConf,
                                       const unsigned int numEncodedSymbols);

    /**
     *
     * @param
     */
    explicit TransformedSymbolsDecoder(const TransformedSymbolsDecoder &trnsfSubseqDecoder);

    /**
     *
     * @param
     */
    ~TransformedSymbolsDecoder() = default;

    /**
     * @brief Decodes next symbol. Before invoking this, caller should check availability with symbolsAvail().
     * @param
     * @return
     */
    uint64_t decodeNextSymbol(uint64_t *depSymbol);

    /**
     * @brief
     * @return the number of symbols available.
     */
    size_t symbolsAvail() const;

 private:
    /**
     * @brief
     * @param
     * @return
     */
    uint64_t decodeNextSymbolOrder0();

    /**
     * @brief
     * @param
     * @return
     */
    uint64_t decodeNextSymbolOrder1(uint64_t *depSymbol);

    /**
     * @brief
     * @param
     * @return
     */
    uint64_t decodeNextSymbolOrder2();

    unsigned int numEncodedSymbols;
    unsigned int numDecodedSymbols;

    uint8_t outputSymbolSize;
    uint8_t codingSubsymSize;
    uint8_t codingOrder;
    uint64_t subsymMask;
    uint8_t numSubSyms;
    uint8_t numLuts;
    uint8_t numPrvs;

    Reader reader;
    ContextSelector ctxSelector;
    LUTsSubSymbolTransform invLutsSubsymTrnsfm;

    bool diffEnabled;
    bool customCmaxTU;
    uint64_t defaultCmax;

    paramcabac::BinarizationParameters::BinarizationId binID;
    std::vector<unsigned int> binParams;
    binFunc binarizor;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
