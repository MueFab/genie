/**
 * @file decoder.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Decoder class for decoding quality values using CALQ.
 *
 * This file contains the declaration of the `Decoder` class, which provides functionalities
 * for decoding quality values encoded using the CALQ algorithm. It utilizes the quality value
 * coding parameters defined in `paramqv1` and leverages the internal decoding structures to
 * reconstruct the quality values for aligned and unaligned sequences.
 *
 * @details The `Decoder` class extends the base `core::QVDecoder` and implements the process method
 * for handling both aligned and unaligned sequences. It uses various utility methods to extract
 * and decode the quality value data from the GENIE framework.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_DECODER_H_
#define SRC_GENIE_QUALITY_CALQ_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/qv-decoder.h"
#include "genie/quality/calq/calq_coder.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::calq {

/**
 * @brief Decoder for quality values using the CALQ algorithm.
 *
 * The `Decoder` class is responsible for decoding quality values encoded with the CALQ algorithm.
 * It supports decoding both aligned and unaligned sequences, leveraging side information such as
 * positions and CIGAR strings. The `Decoder` class uses the quality value coding configurations
 * defined in the `paramqv1::QualityValues1` class to guide the reconstruction process.
 */
class Decoder : public core::QVDecoder {
 private:
    /**
     * @brief Checks if the descriptor represents aligned sequences.
     *
     * This method examines the given descriptor to determine whether it represents
     * aligned sequences. Aligned sequences have specific CIGAR strings and positions
     * that need to be considered during decoding.
     *
     * @param desc The access unit descriptor to check.
     * @return True if the descriptor represents aligned sequences, false otherwise.
     */
    static bool isAligned(const core::AccessUnit::Descriptor& desc);

    /**
     * @brief Decodes quality values for aligned sequences.
     *
     * This method decodes quality values for aligned sequences using the provided
     * configuration parameters and side information. The output is a list of decoded
     * quality values corresponding to each sequence.
     *
     * @param param The quality value coding configuration.
     * @param ecigar_vec The encoded CIGAR strings for the aligned sequences.
     * @param positions The mapping positions for the aligned sequences.
     * @param desc The access unit descriptor containing the encoded data.
     * @return A vector of decoded quality value strings.
     */
    static std::vector<std::string> decodeAligned(const quality::paramqv1::QualityValues1& param,
                                                  const std::vector<std::string>& ecigar_vec,
                                                  const std::vector<uint64_t>& positions,
                                                  core::AccessUnit::Descriptor& desc);

    /**
     * @brief Decodes quality values for unaligned sequences.
     *
     * This method handles the decoding of quality values for unaligned sequences, where no
     * reference or mapping positions are available. It reconstructs the quality values
     * using the internal structure of the descriptor.
     *
     * @param param The quality value coding configuration.
     * @param ecigar_vec The encoded CIGAR strings (or empty strings) for the unaligned sequences.
     * @param desc The access unit descriptor containing the encoded data.
     * @return A vector of decoded quality value strings.
     */
    static std::vector<std::string> decodeUnaligned(const quality::paramqv1::QualityValues1& param,
                                                    const std::vector<std::string>& ecigar_vec,
                                                    core::AccessUnit::Descriptor& desc);

    /**
     * @brief Fills the decoding block structure from the descriptor.
     *
     * This method populates the internal `calq::DecodingBlock` structure using the encoded
     * data from the given descriptor. It extracts the quantizer indices, step indices, and
     * other relevant data needed for the decoding process.
     *
     * @param input The `DecodingBlock` structure to be filled.
     * @param desc The access unit descriptor containing the encoded data.
     * @param param The quality value coding configuration.
     */
    static void fillInput(calq::DecodingBlock& input, core::AccessUnit::Descriptor& desc,
                          const quality::paramqv1::QualityValues1& param);

 public:
    /**
     * @brief Processes the given descriptor and decodes the quality values.
     *
     * This method implements the core functionality for decoding quality values.
     * It determines whether the sequences are aligned or unaligned and applies the
     * appropriate decoding strategy. The method returns the decoded quality values
     * along with performance statistics.
     *
     * @param param The quality value coding configuration to use.
     * @param ecigar_vec The encoded CIGAR strings for the sequences.
     * @param positions The mapping positions for the aligned sequences (empty if unaligned).
     * @param desc The access unit descriptor containing the encoded data.
     * @return A tuple containing the decoded quality value strings and performance statistics.
     */
    std::tuple<std::vector<std::string>, core::stats::PerfStats> process(const core::parameter::QualityValues& param,
                                                                         const std::vector<std::string>& ecigar_vec,
                                                                         const std::vector<uint64_t>& positions,
                                                                         core::AccessUnit::Descriptor& desc) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::calq

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
