/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_BASECODER_DECODERSTUB_H_
#define SRC_GENIE_READ_BASECODER_DECODERSTUB_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/read-decoder.h"
#include "genie/read/basecoder/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

/**
 * @brief Common stub for all aligned record decoders
 */
class DecoderStub : public core::ReadDecoder {
 protected:
    /**
     * @brief
     */
    struct DecodingState {
        core::AccessUnit::Descriptor qvStream;                    //!< @brief
        core::AccessUnit::Descriptor nameStream;                  //!< @brief
        size_t numRecords;                                        //!< @brief
        size_t numSegments;                                       //!< @brief
        size_t ref;                                               //!< @brief
        std::vector<std::string> ecigars;                         //!< @brief
        std::unique_ptr<core::parameter::QualityValues> qvparam;  //!< @brief

        /**
         * @brief
         * @param t_data
         */
        explicit DecodingState(core::AccessUnit& t_data);

        /**
         * @brief
         */
        virtual ~DecodingState() = default;
    };

    /**
     * @brief
     * @param t
     * @return
     */
    virtual std::unique_ptr<DecodingState> createDecodingState(core::AccessUnit& t) = 0;

    /**
     * @brief
     * @param meta
     * @param state
     * @return
     */
    virtual std::vector<std::string> getReferences(const basecoder::Decoder::SegmentMeta& meta,
                                                   DecodingState& state) = 0;

    /**
     * @brief
     * @param state
     * @param t_data
     * @return
     */
    virtual core::record::Chunk decodeSequences(DecodingState& state, core::AccessUnit& t_data);

    /**
     * @brief
     * @param state
     * @param chunk
     */
    virtual void decodeNames(DecodingState& state, core::record::Chunk& chunk);

    /**
     * @brief
     * @param state
     * @param chunk
     */
    virtual void decodeQualities(DecodingState& state, core::record::Chunk& chunk);

    /**
     * @brief
     * @param rec
     * @param cig_vec
     */
    virtual void addECigar(const core::record::Record& rec, std::vector<std::string>& cig_vec);

    /**
     * @brief
     * @param state
     * @param rec
     */
    virtual void recordDecodedHook(DecodingState&, const core::record::Record&) {}

 public:
    /**
     * @brief
     * @param t
     * @param id
     */
    void flowIn(core::AccessUnit&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_BASECODER_DECODERSTUB_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
