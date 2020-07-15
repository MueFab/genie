/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LL_DECODER_H
#define GENIE_LL_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/read-decoder.h>
#include <ref-decoder.h>

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

    std::string decode(core::AccessUnit&& t) override;
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

#endif  // GENIE_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
