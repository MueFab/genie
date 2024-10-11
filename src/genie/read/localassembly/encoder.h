/**
 * @file encoder.h
 * @brief Header file for the `Encoder` class in the `localassembly` namespace.
 *
 * This file defines the `Encoder` class for handling the encoding of records
 * using the Local Assembly coding scheme. It extends the basecoder `EncoderStub`
 * class and provides additional functionality for managing local references
 * during the encoding process.
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_
#define SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include "genie/core/read-encoder.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/read/basecoder/encoderstub.h"
#include "genie/read/localassembly/local-reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::localassembly {

/**
 * @brief Encoder class for handling local assembly based encoding.
 *
 * This class extends the `basecoder::EncoderStub` to implement the local assembly
 * encoding process. It manages the local reference sequences used to correctly
 * encode the record data in the GENIE framework.
 */
class Encoder : public basecoder::EncoderStub {
 private:
    bool debug;  //!< @brief Flag for enabling debug output during encoding.

    /**
     * @brief Internal encoding state (not exposed publicly).
     */
    struct LAEncodingState : public basecoder::EncoderStub::EncodingState {
        /**
         * @brief Construct from raw data.
         * @param data Chunk of data to construct from.
         * @param _cr_buf_max_size  LA parameter.
         */
        explicit LAEncodingState(const core::record::Chunk& data, uint32_t _cr_buf_max_size);
        LocalReference refCoder;  //!< @brief Local assembly handler.
    };

    /**
     * @brief Print current local assembly state.
     * @param state Current state of the encoder.
     * @param ref1 Reference sequence of the first segment.
     * @param ref2 Reference sequence of the second segment.
     * @param r Current record being processed.
     */
    void printDebug(const LAEncodingState& state, const std::string& ref1, const std::string& ref2,
                    const core::record::Record& r) const;

    /**
     * @brief Pack encoded data into an access unit.
     * @param id Multithreading sync ID.
     * @param qv Encoded quality values.
     * @param rname Encoded record names.
     * @param state Encoding state.
     * @return Assembled access unit with parameters.
     */
    core::AccessUnit pack(size_t id, core::QVEncoder::QVCoded qv, core::AccessUnit::Descriptor rname,
                          EncodingState& state) override;

    /**
     * @brief Calculate references for record from local assembly.
     * @param r Record to calculate for.
     * @param state Encoding state.
     * @return Pair of references. If unpaired, second reference remains empty.
     */
    std::pair<std::string, std::string> getReferences(const core::record::Record& r, EncodingState& state) override;

    /**
     * @brief Create a new encoding state from a chunk of data.
     * @param data Chunk to create from.
     * @return A unique pointer to an `EncodingState` object.
     */
    [[nodiscard]] std::unique_ptr<EncodingState> createState(const core::record::Chunk& data) const override;

 public:
    /**
     * @brief Create an encoder with specified parameters.
     * @param _debug If true, debug information is printed.
     * @param _write_raw Flag for enabling raw write mode.
     */
    Encoder(bool _debug, bool _write_raw);

    /**
     * @brief Downgrade class N to class M and then proceed with encoding.
     * @param t Data chunk to process.
     * @param id Multithreading sync ID.
     */
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
