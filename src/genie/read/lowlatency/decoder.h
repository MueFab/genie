/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOWLATENCY_DECODER_H_
#define SRC_GENIE_READ_LOWLATENCY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/read-decoder.h"
#include "genie/core/ref-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace lowlatency {

/**
 *
 */
class Decoder : public core::ReadDecoder, public core::RefDecoder {
 private:
 public:
    /**
     *
     * @param t
     * @return
     */
    std::string decode(core::AccessUnit&& t) override;

    /**
     *
     * @param t
     * @return
     */
    core::record::Chunk decode_common(core::AccessUnit&& t);

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(core::AccessUnit&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lowlatency
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOWLATENCY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
