/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 * @brief Base class for annotation entropy encoders
 * @details Provides a common interface for entropy encoders that work with
 * annotation data structures, particularly TypedData objects.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BASE_ENCODER_H_
#define SRC_GENIE_ENTROPY_BASE_ENCODER_H_

// -----------------------------------------------------------------------------

#include <vector>
//#include "genie/core/entropy_encoder.h"
#include "genie/core/record/annotation_access_unit/TypedData.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace base {

/**
 * @brief Base class for annotation-specific entropy encoders
 * @details Provides a standardized interface for encoders that process
 * TypedData structures, commonly used in annotation encoding.
 */
class Encoder {
 public:
    /**
     * @brief Virtual destructor
     */
    virtual ~Encoder() = default;

    /**
     * @brief Set input data for encoding
     * @param index Input index (typically 0)
     * @param input Input typed data
     */
    void setInput(uint8_t inVarID, const core::record::annotation_access_unit::TypedData& data);

    /**
     * @brief Perform the encoding operation
     */
    virtual void encode() = 0;

    /**
     * @brief Get encoded output
     * @param index Output index (0 = symbols, 1 = dimensions, etc.)
     * @return Reference to output typed data
     */
    virtual void decode() = 0;

    /**
     * @brief Get encoded output
     * @param index Output index (0 = symbols, 1 = dimensions, etc.)
     * @return Reference to output typed data
     */
    const core::record::annotation_access_unit::TypedData& getOutput(uint8_t outVarID) const;

 protected:
    Encoder() = default;
    std::vector<core::record::annotation_access_unit::TypedData> inputs;   ///< Input data
    std::vector<core::record::annotation_access_unit::TypedData> outputs;  ///< Output data
};

// -----------------------------------------------------------------------------

}  // namespace base
}  // namespace entropy
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BASE_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------