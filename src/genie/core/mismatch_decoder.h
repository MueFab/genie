/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_MISMATCH_DECODER_H_
#define SRC_GENIE_CORE_MISMATCH_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class MismatchDecoder {
 public:
    /**
     * @brief
     */
    virtual ~MismatchDecoder() = default;

    /**
     * @brief
     * @param ref
     * @return
     */
    virtual uint64_t DecodeMismatch(uint64_t ref) = 0;

    /**
     * @brief
     * @return
     */
    virtual bool DataLeft() const = 0;

    /**
     * @brief
     * @return
     */
    virtual std::unique_ptr<MismatchDecoder> Copy() const = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_MISMATCH_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
