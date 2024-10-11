/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the `TransformedSymbolsDecoder` class for decoding transformed symbols.
 *
 * This file defines the `TransformedSymbolsDecoder` class, which is responsible for decoding
 * symbols that have undergone transformations like binarization, context modeling, and
 * LUT-based reordering. The class supports decoding for different coding orders and provides
 * mechanisms for handling various CABAC-related operations, such as context selection and
 * symbol decoding.
 *
 * @details The `TransformedSymbolsDecoder` class offers various methods to handle symbol
 * decoding in different transformation orders (Order-0, Order-1, and Order-2). It utilizes
 * bitstream readers, context selectors, and LUT-based transformations to revert the transformed
 * symbols back to their original form. This class is designed to operate with MPEG-G CABAC
 * configurations.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>                                           // For using std::vector in class properties.
#include "genie/entropy/gabac/context-selector.h"           // For context-based decoding.
#include "genie/entropy/gabac/decode-transformed-subseq.h"  // For symbol transformation configurations.
#include "genie/entropy/gabac/luts-subsymbol-transform.h"   // For LUT transformations and subsymbol operations.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Class for decoding symbols that have undergone various transformations.
 *
 * The `TransformedSymbolsDecoder` class provides the capability to decode symbols that were
 * transformed using techniques like binarization, context modeling, and LUT-based subsymbol
 * reordering. It supports various orders of transformation, ranging from Order-0 (no dependency)
 * to Order-2 (inter-symbol dependencies). The class maintains internal state for the number of
 * symbols encoded and decoded, as well as configurations for decoding parameters.
 */
class TransformedSymbolsDecoder {
 public:
    /**
     * @brief Constructs a `TransformedSymbolsDecoder` object from a given bitstream and configuration.
     *
     * This constructor initializes the decoder using a reference to the input `bitstream` and the
     * `trnsfSubseqConf` configuration, which specifies how the subsequence was encoded. The number of
     * symbols to be decoded is set using `numEncodedSymbols`.
     *
     * @param bitstream Pointer to the input bitstream containing encoded data.
     * @param trnsfSubseqConf Configuration for the transformed subsequence.
     * @param numEncodedSymbols The total number of symbols encoded in the input stream.
     */
    explicit TransformedSymbolsDecoder(util::DataBlock* bitstream, const paramcabac::TransformedSubSeq& trnsfSubseqConf,
                                       unsigned int numEncodedSymbols);

    /**
     * @brief Copy constructor for `TransformedSymbolsDecoder`.
     *
     * Creates a copy of the specified `TransformedSymbolsDecoder` object.
     *
     * @param trnsfSubseqDecoder The `TransformedSymbolsDecoder` object to copy.
     */
    TransformedSymbolsDecoder(const TransformedSymbolsDecoder& trnsfSubseqDecoder);

    /**
     * @brief Default destructor.
     *
     * Releases any allocated resources and performs cleanup operations.
     */
    ~TransformedSymbolsDecoder() = default;

    /**
     * @brief Decodes the next symbol from the bitstream.
     *
     * This function decodes the next symbol from the input bitstream, using the current
     * configuration and context models. If the transformation order is greater than 0, the function
     * uses the provided `depSymbol` as a dependency for context modeling.
     *
     * @param depSymbol Pointer to the dependency symbol (if any) for context-based decoding.
     * @return The value of the decoded symbol.
     */
    uint64_t decodeNextSymbol(uint64_t* depSymbol);

    /**
     * @brief Retrieves the number of symbols remaining for decoding.
     *
     * This function returns the number of symbols that are yet to be decoded from the input
     * bitstream.
     *
     * @return The number of symbols available for decoding.
     */
    [[nodiscard]] size_t symbolsAvail() const;

 private:
    /**
     * @brief Decodes the next symbol using Order-0 transformation (no dependency).
     *
     * This function handles decoding for Order-0 transformations, where symbols are
     * independent of each other. It reads the symbols directly from the bitstream.
     *
     * @return The value of the decoded symbol.
     */
    uint64_t decodeNextSymbolOrder0();

    /**
     * @brief Decodes the next symbol using Order-1 transformation (dependency on previous symbol).
     *
     * This function handles decoding for Order-1 transformations, where each symbol depends
     * on the previous symbol (depSymbol). It utilizes the LUT and context models for decoding.
     *
     * @param depSymbol Pointer to the dependency symbol for context modeling.
     * @return The value of the decoded symbol.
     */
    uint64_t decodeNextSymbolOrder1(const uint64_t* depSymbol);

    /**
     * @brief Decodes the next symbol using Order-2 transformation (inter-symbol dependencies).
     *
     * This function handles decoding for Order-2 transformations, where symbols depend on
     * both previous symbols and additional parameters.
     *
     * @return The value of the decoded symbol.
     */
    uint64_t decodeNextSymbolOrder2();

    // Member variables for maintaining decoding state and configurations.
    unsigned int numEncodedSymbols;  //!< @brief Total number of symbols to be decoded.
    unsigned int numDecodedSymbols;  //!< @brief Number of symbols decoded so far.

    uint8_t outputSymbolSize;  //!< @brief Size of the output symbol in bits.
    uint8_t codingSubsymSize;  //!< @brief Size of the coding subsymbol in bits.
    uint8_t codingOrder;       //!< @brief Coding order (0, 1, or 2).
    uint64_t subsymMask;       //!< @brief Mask used for extracting subsymbols.
    uint8_t numSubSyms;        //!< @brief Number of subsymbols per symbol.
    uint8_t numLuts;           //!< @brief Number of LUTs used.
    uint8_t numPrvs;           //!< @brief Number of previous symbols used for dependencies.

    Reader reader;                               //!< @brief Bitstream reader for CABAC decoding.
    ContextSelector ctxSelector;                 //!< @brief Context selector for CABAC decoding.
    LUTsSubSymbolTransform invLutsSubsymTrnsfm;  //!< @brief LUT-based subsymbol transformation handler.

    bool diffEnabled;      //!< @brief Flag to indicate if differential coding is enabled.
    bool customCmaxTU;     //!< @brief Flag to indicate if custom CMax is used for TU binarization.
    uint64_t defaultCmax;  //!< @brief Default CMax value used for CABAC decoding.

    paramcabac::BinarizationParameters::BinarizationId binID;  //!< @brief Binarization method used for decoding.
    std::vector<unsigned int> binParams;  //!< @brief Parameters for the specified binarization method.
    binFunc binarizor;                    //!< @brief Binarization function pointer.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
