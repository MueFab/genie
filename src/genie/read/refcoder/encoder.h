/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_REFCODER_ENCODER_H_
#define SRC_GENIE_READ_REFCODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/read-encoder.h"
#include "genie/read/basecoder/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace refcoder {

/**
 * @brief Module using a reference sequence to encode aligned reads
 */
class Encoder : public core::ReadEncoder {
 private:
    /**
     * @brief Internal encoding state (not exposed publicly)
     */
    struct EncodingState {
        /**
         * @brief Construct internal variables with default values
         * @param startingPos Starting position of access unit
         */
        explicit EncodingState(uint64_t startingPos);
        basecoder::Encoder readCoder;       //!< @brief Generating the descriptor streams
        bool pairedEnd;                     //!< @brief Current guess regarding pairing
        size_t readLength;                  //!< @brief Current guess regarding read length
        core::record::ClassType classType;  //!< @brief Current guess regarding class type
        uint64_t minPos;                    //!<
        uint64_t maxPos;                    //!<
    };

    /**
     * @brief Updates the assembly itself. The local reference will be updated and descriptor streams extended
     * @param r The record we will be updating with
     * @param srec Paired alignment extracted from the record
     * @param state Current local assembly state
     */
    void updateAssembly(const core::record::Record& r, EncodingState& state,
                        const core::ReferenceManager::ReferenceExcerpt& excerpt) const;

    /**
     * @brief Checks if there is an additional alignment available and converts it into the correct format
     * @param state Current local assembly state
     * @param r Current record
     * @return Preprocessed alignment for pair
     */
    const core::record::alignment_split::SameRec& getPairedAlignment(const Encoder::EncodingState& state,
                                                                     const core::record::Record& r) const;

    /**
     * @brief Update guesses regarding read length, pairing ...
     * @param state Current local assembly state
     * @param r Current input record
     */
    void updateGuesses(const core::record::Record& r, Encoder::EncodingState& state) const;

    /**
     * @brief Generate a full RawAccessUnit and parameter set as a last step of encoding
     * @param id Block identifier for the current data
     * @param state The state used throughout the current encoding
     * @return RawAccessUnit ready for entropy core
     */
    core::AccessUnit pack(size_t id, uint16_t ref, uint8_t qv_depth,
                          std::unique_ptr<core::parameter::QualityValues> qvparam, core::record::ClassType type,
                          Encoder::EncodingState& state) const;

 public:
    /**
     * @brief Process one chunk of data
     * @param t The input data
     * @param id The current block identifier
     */
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_REFCODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
