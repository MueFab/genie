/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LL_ENCODER_H
#define GENIE_LL_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/read-encoder.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace lowlatency {

/**
 *
 */
class Encoder : public core::ReadEncoder {
   private:
    /**
     *
     */
    struct LLState {
        size_t readLength;         //!<
        bool pairedEnd;            //!<
        core::AccessUnit streams;  //!<
        bool refOnly;
    };

   public:
    /**
     *
     * @param t
     * @param id
     */
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;

    /**
     *
     * @param id
     * @param qv_depth
     * @param qvparam
     * @param state
     * @return
     */
    core::AccessUnit pack(const util::Section& id, uint8_t qv_depth,
                          std::unique_ptr<core::parameter::QualityValues> qvparam, LLState& state) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lowlatency
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
