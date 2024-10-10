/**
 * @file encoder.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Encoder class for handling quality value encoding with parameterized configurations.
 *
 * This file contains the declaration of the `Encoder` class, which is a specialized implementation
 * of the `QVEncoder` interface within the GENIE framework. The `Encoder` class is responsible for
 * processing quality values in genomic records and applying parameterized encoding techniques
 * based on the GENIE specifications.
 *
 * @details The `Encoder` class provides a comprehensive approach to encoding quality values in
 * genomic data, supporting both aligned and unaligned segments. It utilizes the `paramqv1` namespace
 * to configure quality value parameters and encode segments according to the format required by
 * the GENIE framework.
 */

#ifndef SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_H_
#define SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/qv-encoder.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::qvwriteout {

/**
 * @brief Implements the quality value encoding functionality with parameterized configurations.
 *
 * The `Encoder` class is responsible for encoding quality values in genomic records using
 * a parameterized approach. It handles both aligned and unaligned segments, applying different
 * encoding strategies based on the segment type. The encoded data is organized into descriptors
 * that can be used for further processing within the GENIE framework.
 */
class Encoder : public core::QVEncoder {
 private:
    /**
     * @brief Sets up the quality value parameters for a given record chunk.
     *
     * This static method initializes the quality value parameters based on the input
     * record chunk and updates the `Descriptor` object accordingly. It configures the
     * necessary quality value settings to ensure that the data is encoded correctly.
     *
     * @param rec The chunk of genomic records to be processed.
     * @param param The `QualityValues1` object that holds the parameter configurations.
     * @param desc The descriptor to store the encoded parameters.
     */
    static void setUpParameters(const core::record::Chunk& rec, paramqv1::QualityValues1& param,
                                core::AccessUnit::Descriptor& desc);

    /**
     * @brief Encodes an aligned segment's quality values into the descriptor.
     *
     * This static method encodes the quality values of an aligned segment using the given
     * extended CIGAR string (`ecigar`). The encoded values are stored in the provided descriptor.
     *
     * @param s The segment to be encoded.
     * @param ecigar The extended CIGAR string describing the alignment.
     * @param desc The descriptor to store the encoded segment data.
     */
    static void encodeAlignedSegment(const core::record::Segment& s, const std::string& ecigar,
                                     core::AccessUnit::Descriptor& desc);

    /**
     * @brief Encodes an unaligned segment's quality values into the descriptor.
     *
     * This static method encodes the quality values of an unaligned segment and stores
     * the data in the provided descriptor. Unaligned segments have a different encoding
     * strategy compared to aligned segments.
     *
     * @param s The segment to be encoded.
     * @param desc The descriptor to store the encoded segment data.
     */
    static void encodeUnalignedSegment(const core::record::Segment& s, core::AccessUnit::Descriptor& desc);

 public:
    /**
     * @brief Processes a chunk of genomic records and encodes their quality values.
     *
     * This method takes a chunk of genomic records, extracts the quality values, and encodes
     * them using the appropriate strategies for aligned and unaligned segments. It returns a
     * `QVCoded` object containing the encoded quality values.
     *
     * @param rec The chunk of genomic records to be processed.
     * @return A `QVCoded` object containing the encoded quality values.
     */
    core::QVEncoder::QVCoded process(const core::record::Chunk& rec) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::qvwriteout

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
