/**
 * @file decoder.h
 * @brief Header file for the `Decoder` class in the `localassembly` namespace.
 *
 * This file defines the `Decoder` class for handling the decoding of records
 * using the Local Assembly coding scheme. It extends the basecoder `DecoderStub`
 * class and provides additional functionality for managing local references
 * during the decoding process.
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_
#define SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/read/basecoder/decoderstub.h"
#include "genie/read/localassembly/local-reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::localassembly {

/**
 * @brief Decoder class for handling local assembly based decoding.
 *
 * This class extends the `basecoder::DecoderStub` to implement the local assembly
 * decoding process. It manages the local reference sequences used to correctly
 * decode the record data in the GENIE framework.
 */
class Decoder : public basecoder::DecoderStub {
 private:
    /**
     * @brief Create a specialized decoding state for the local assembly decoder.
     * @param t Incoming access unit data.
     * @return A unique pointer to the decoding state.
     */
    std::unique_ptr<DecodingState> createDecodingState(core::AccessUnit& t) override;

    /**
     * @brief Get reference sequences for a given segment.
     * @param meta Metadata of the segment.
     * @param state Current decoding state.
     * @return Vector of reference sequences.
     */
    std::vector<std::string> getReferences(const basecoder::Decoder::SegmentMeta& meta, DecodingState& state) override;

    /**
     * @brief Hook called after each record is decoded.
     * @param state Current decoding state.
     * @param rec Decoded record.
     */
    void recordDecodedHook(basecoder::DecoderStub::DecodingState& state, const core::record::Record& rec) override;

    /**
     * @brief Specialized state structure for local assembly decoding.
     */
    struct LADecodingState : public DecodingState {
        LocalReference refEncoder;  //!< @brief Handles local reference sequence management.

        /**
         * @brief Constructs the decoding state with the provided access unit.
         * @param t_data Access unit to initialize the state.
         */
        explicit LADecodingState(core::AccessUnit& t_data);
    };

 public:
    /**
     * @brief Default constructor for the `Decoder` class.
     */
    Decoder() = default;

    /**
     * @brief Default destructor for the `Decoder` class.
     */
    ~Decoder() override = default;
};

}  // namespace genie::read::localassembly

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
