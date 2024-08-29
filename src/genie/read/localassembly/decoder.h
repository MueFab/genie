/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
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
     * @param state
     * @param rec
     */
    void recordDecodedHook(basecoder::DecoderStub::DecodingState& state, const core::record::Record& rec) override;

    /**
     * @brief
     */
    struct LADecodingState : public DecodingState {
        LocalReference refEncoder;  //!< @brief
        /**
         * @brief
         * @param t_data
         */
        explicit LADecodingState(core::AccessUnit& t_data);
    };

 public:
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
