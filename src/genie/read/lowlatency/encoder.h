/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOWLATENCY_ENCODER_H_
#define SRC_GENIE_READ_LOWLATENCY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/read-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::lowlatency {

/**
 * @brief
 */
class Encoder : public core::ReadEncoder {
 private:
    /**
     * @brief
     */
    struct LLState {
        size_t readLength;         //!< @brief
        bool pairedEnd;            //!< @brief
        core::AccessUnit streams;  //!< @brief
        bool refOnly;              //!< @brief
    };

 public:
    /**
     * @brief
     * @param t
     * @param id
     */
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;

    /**
     * @brief
     * @param id
     * @param qv_depth
     * @param qvparam
     * @param state
     * @return
     */
    static core::AccessUnit pack(const util::Section& id, uint8_t qv_depth,
                                 std::unique_ptr<core::parameter::QualityValues> qvparam, LLState& state);

    /**
     * @brief
     * @param write_raw
     */
    explicit Encoder(bool write_raw) : core::ReadEncoder(write_raw) {}
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::lowlatency

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOWLATENCY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
