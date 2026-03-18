/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder_factory.h
 * @brief Factory class for creating annotation entropy encoders
 * @details Provides a common interface for creating entropy encoders that work with
 * annotation data structures, particularly TypedData objects.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_FACTORY_ENCODER_FACTORY_H_
#define SRC_GENIE_ENTROPY_FACTORY_ENCODER_FACTORY_H_

// -----------------------------------------------------------------------------

#include <memory>
#include "genie/entropy/base/encoder.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_access_unit/TypedData.h"
// -----------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace factory {

/**
 * Factory for creating entropy encoder instances.
 * This factory breaks the circular dependency by separating
 * the creation logic from the base class.
 */
class EncoderFactory {
 public:
    /**
     * Creates an encoder instance based on the algorithm ID.
     * @param algorithmID The algorithm to use for encoding
     * @param parameters Algorithm-specific parameters
     * @return A unique pointer to the created encoder
     * @throws std::runtime_error if the algorithm ID is not supported
     */
    static std::unique_ptr<base::Encoder> createEncoder(
        core::AlgoID algorithmID,
        const core::record::annotation_parameter_set::AlgorithmParameters& parameters);
};

// -----------------------------------------------------------------------------

}  // namespace factory
}  // namespace entropy
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_FACTORY_ENCODER_FACTORY_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
