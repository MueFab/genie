/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_REFCODER_DECODER_H_
#define SRC_GENIE_READ_REFCODER_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/read-decoder.h"
#include "genie/read/basecoder/decoderstub.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::refcoder {

/**
 * @brief
 */
class Decoder : public basecoder::DecoderStub {
 private:
    /**
     * @brief
     * @param t
     * @return
     */
    std::unique_ptr<DecodingState> createDecodingState(core::AccessUnit& t) override;

    /**
     * @brief
     * @param meta
     * @param state
     * @return
     */
    std::vector<std::string> getReferences(const basecoder::Decoder::SegmentMeta& meta, DecodingState& state) override;

    /**
     * @brief
     */
    struct RefDecodingState : public DecodingState {
        core::ReferenceManager::ReferenceExcerpt refExcerpt;
        /**
         * @brief
         * @param t_data
         */
        explicit RefDecodingState(core::AccessUnit& t_data);
    };

 public:
};
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::refcoder

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_REFCODER_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
