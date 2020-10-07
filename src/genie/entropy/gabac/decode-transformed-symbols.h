/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_TRANSFORMED_SYMBOLS_DECODER_H
#define GABAC_TRANSFORMED_SYMBOLS_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "decode-transformed-subseq.h"
#include "context-selector.h"
#include "luts-subsymbol-transform.h"

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
    explicit TransformedSymbolsDecoder(util::DataBlock *bitstream,
                                       const paramcabac::TransformedSubSeq &trnsfSubseqConf,
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
     * @brief
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

#endif  // GABAC_TRANSFORMED_SYMBOLS_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------