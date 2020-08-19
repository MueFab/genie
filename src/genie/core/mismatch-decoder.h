/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MISMATCH_DECODER_H
#define GENIE_MISMATCH_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

class AccessUnitSubsequence;

/**
 *
 */
class MismatchDecoder {
   public:
    /**
     *
     */
    virtual ~MismatchDecoder() = default;

    /**
     *
     * @param ref
     * @return
     */
    virtual uint64_t decodeMismatch(uint64_t ref) = 0;

    /**
     *
     * @return
     */
    virtual bool dataLeft() const = 0;

    /**
     *
     * @return
     */
    virtual std::unique_ptr<MismatchDecoder> copy() const = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MISMATCH_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------