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

namespace genie::entropy::gabac {

/**
 * @brief
 */
class TransformedSymbolsDecoder {
 public:
    /**
     * @brief
     * @param bitstream
     * @param trnsfSubseqConf
     * @param numEncodedSymbols
     */
    explicit TransformedSymbolsDecoder(util::DataBlock *bitstream, const paramcabac::TransformedSubSeq &trnsfSubseqConf,
                                       const unsigned int numEncodedSymbols);

    /**
     * @brief
     * @param trnsfSubseqDecoder
     */
    explicit TransformedSymbolsDecoder(const TransformedSymbolsDecoder &trnsfSubseqDecoder);

    /**
     * @brief
     */
    ~TransformedSymbolsDecoder() = default;

    /**
     * @brief Decodes next symbol. Before invoking this, caller should check availability with symbolsAvail().
     * @param depSymbol
     * @return
     */
    uint64_t decodeNextSymbol(uint64_t *depSymbol);

    /**
     * @brief
     * @return the number of symbols available.
     */
    [[nodiscard]] size_t symbolsAvail() const;

 private:
    /**
     * @brief
     * @return
     */
    uint64_t decodeNextSymbolOrder0();

    /**
     * @brief
     * @param depSymbol
     * @return
     */
    uint64_t decodeNextSymbolOrder1(uint64_t *depSymbol);

    /**
     * @brief
     * @return
     */
    uint64_t decodeNextSymbolOrder2();

    unsigned int numEncodedSymbols;  //!< @brief
    unsigned int numDecodedSymbols;  //!< @brief

    uint8_t outputSymbolSize;  //!< @brief
    uint8_t codingSubsymSize;  //!< @brief
    uint8_t codingOrder;       //!< @brief
    uint64_t subsymMask;       //!< @brief
    uint8_t numSubSyms;        //!< @brief
    uint8_t numLuts;           //!< @brief
    uint8_t numPrvs;           //!< @brief

    Reader reader;                               //!< @brief
    ContextSelector ctxSelector;                 //!< @brief
    LUTsSubSymbolTransform invLutsSubsymTrnsfm;  //!< @brief

    bool diffEnabled;      //!< @brief
    bool customCmaxTU;     //!< @brief
    uint64_t defaultCmax;  //!< @brief

    paramcabac::BinarizationParameters::BinarizationId binID;  //!< @brief
    std::vector<unsigned int> binParams;                       //!< @brief
    binFunc binarizor;                                         //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
