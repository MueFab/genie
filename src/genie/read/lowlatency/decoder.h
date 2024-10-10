/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOWLATENCY_DECODER_H_
#define SRC_GENIE_READ_LOWLATENCY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include "genie/core/read-decoder.h"
#include "genie/core/ref-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::lowlatency {

/**
 * @brief
 */
class Decoder : public core::ReadDecoder, public core::RefDecoder {
 private:
 public:
    /**
     * @brief
     * @param t
     * @return
     */
    std::string decode(core::AccessUnit&& t) override;

    /**
     * @brief
     * @param t
     * @return
     */
    core::record::Chunk decode_common(core::AccessUnit&& t);

    /**
     * @brief
     * @param t
     * @param id
     */
    void flowIn(core::AccessUnit&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::lowlatency

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOWLATENCY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
