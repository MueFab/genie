/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_REFCODER_ENCODER_H_
#define SRC_GENIE_READ_REFCODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/stop_watch.h>
#include <memory>
#include <string>
#include <utility>
#include "genie/core/read-encoder.h"
#include "genie/read/basecoder/encoderstub.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace refcoder {

/**
 * @brief Module using a reference sequence to encode aligned reads
 */
class Encoder : public basecoder::EncoderStub {
 private:
    /**
     * @brief Internal encoding state (not exposed publicly)
     */
    struct RefEncodingState : public EncoderStub::EncodingState {
        /**
         * @brief Construct from chunk of data
         * @param data Chunk of data
         */
        explicit RefEncodingState(const core::record::Chunk& data);
        core::ReferenceManager::ReferenceExcerpt excerpt;  //!< @brief Reference information of chunk
    };

    /**
     * @brief Get references from existing reference sequence
     * @param r Record to get references for
     * @param state State to use for encoding
     * @return Reference sequences
     */
    std::pair<std::string, std::string> getReferences(const core::record::Record& r, EncodingState& state) override;

    /**
     * @brief Construct the RefEncodingState
     * @param data Chunk of data to construct form
     * @return RefEncodingState
     */
    std::unique_ptr<EncodingState> createState(const core::record::Chunk& data) const override;

 public:
    /**
     * @brief
     * @param raw_write
     */
    explicit Encoder(bool raw_write) : basecoder::EncoderStub(raw_write) {}
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_REFCODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
