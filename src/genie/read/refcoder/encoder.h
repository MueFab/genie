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
 *
 */
class Encoder : public core::ReadEncoder {
 private:
    struct State {
        size_t readLength{};
        bool pairedEnd{};
        size_t minPos{};
        size_t maxPos{};
        basecoder::Encoder readCoder;
        explicit State(size_t start) : readCoder((int32_t)start) {}
    };

    void updateAssembly(const core::record::Record& r, State& state,
                        const core::ReferenceManager::ReferenceExcerpt& excerpt) const;
    static const core::record::alignment_split::SameRec& getPairedAlignment(const core::record::Record& r);
    core::AccessUnit pack(size_t id, uint16_t ref, uint8_t qv_depth,
                          std::unique_ptr<core::parameter::QualityValues> qvparam, core::record::ClassType type,
                          State& state) const;

 public:
    /**
     *
     */
    Encoder();

    /**
     *
     */
    void flowIn(core::record::Chunk&&, const util::Section&) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_REFCODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
