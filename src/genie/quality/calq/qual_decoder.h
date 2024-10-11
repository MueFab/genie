/**
 * @file qual_decoder.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the QualDecoder class and supporting structures for decoding quality values.
 *
 * This file contains the declaration of the `QualDecoder` class, which provides methods
 * for decoding mapped and unmapped genomic quality values using quantizers and encoded quality
 * blocks. It handles the conversion of encoded quality values back to their original form using
 * various quantization strategies and maintains state information for each read.
 *
 * @details The `QualDecoder` class is used to decode encoded quality blocks into a format
 * that can be used for further processing or storage. It relies on input structures like
 * `DecodingRead`, `EncodingBlock`, and `DecodingBlock` to maintain and decode quality values.
 * The decoder uses an internal set of quantizers to determine how the quality values should be
 * reconstructed.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_QUAL_DECODER_H_
#define SRC_GENIE_QUALITY_CALQ_QUAL_DECODER_H_

#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "genie/quality/calq/quantizer.h"

namespace genie::quality::calq {

struct DecodingRead;
struct EncodingBlock;
struct DecodingBlock;

/**
 * @brief Stores metadata for decoding a specific read.
 *
 * The `DecodingRead` structure stores the necessary metadata for decoding a single
 * read, including the minimum mapping position (`posMin`) and the corresponding
 * CIGAR string that describes the alignment of the read.
 */
struct DecodingRead {
    uint64_t posMin;    //!< @brief The 1-based leftmost mapping position of the read.
    std::string cigar;  //!< @brief The CIGAR string for the read, describing alignment operations.
};

/**
 * @brief A decoder class for reconstructing quality values from encoded blocks.
 *
 * The `QualDecoder` class is responsible for taking encoded quality values and
 * reconstructing them into their original quality score representations. It uses
 * quantizers stored in the `DecodingBlock` to decode the values according to the
 * specified quantization strategy.
 */
class QualDecoder {
 private:
    uint64_t posOffset_;      //!< @brief Offset for read positions to handle global positions correctly.
    int qualityValueOffset_;  //!< @brief The offset applied to quality values (e.g., 33 for ASCII format).

    std::vector<size_t> qviIdx_;  //!< @brief Indexes of the quantizer values used for decoding.
    std::vector<Quantizer>
        quantizers_;  //!< @brief The quantizers used to decode quality values for different segments.

    EncodingBlock* out;       //!< @brief Pointer to the output structure where decoded quality values are stored.
    const DecodingBlock& in;  //!< @brief The input encoded block containing compressed quality values.

 public:
    /**
     * @brief Constructs a `QualDecoder` with the specified input and output blocks.
     *
     * This constructor initializes a `QualDecoder` object using a reference to the encoded
     * `DecodingBlock` and sets up internal states for decoding. The decoded values are stored
     * in the `EncodingBlock` passed in the `out` parameter.
     *
     * @param in The input encoded block to be decoded.
     * @param positionOffset The starting position offset for read positions.
     * @param qualityOffset The quality value offset to be applied during decoding.
     * @param out Pointer to the output structure where decoded quality values will be stored.
     */
    QualDecoder(const DecodingBlock& in, uint64_t positionOffset, uint8_t qualityOffset, EncodingBlock* out);

    /**
     * @brief Destructor for the `QualDecoder` class.
     *
     * Cleans up the internal states and pointers used by the `QualDecoder`.
     */
    ~QualDecoder();

    /**
     * @brief Decodes a mapped record using information from the encoded block.
     *
     * This function takes a `DecodingRead` structure, extracts relevant metadata,
     * and uses it to decode quality values for a single mapped record. The decoded values
     * are stored in the `EncodingBlock` that was provided during construction.
     *
     * @param samRecord The `DecodingRead` structure containing metadata for the read.
     */
    void decodeMappedRecordFromBlock(const DecodingRead& samRecord);
};

}  // namespace genie::quality::calq

#endif  // SRC_GENIE_QUALITY_CALQ_QUAL_DECODER_H_
