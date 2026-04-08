/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_SER_ENCODER_H_
#define SRC_GENIE_ENTROPY_SER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <sstream>
#include <vector>

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/core/constants.h"
#include "genie/entropy/base/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace ser {

class SERParameters {
 public:
    bool order;

    SERParameters()
        : order(true) {
    }

    explicit SERParameters(bool _order)
        : order(_order) {
    }

    core::record::annotation_parameter_set::AlgorithmParameters convertToAlgorithmParameters() const;

    bool parsAreDefault() const {
        return order == true;
    }
};

class SEREncoder : public base::Encoder {
 public:
    SEREncoder();

    // Main encoding/decoding methods
    void encode();
    void decode();

    void configure(const SERParameters serParameters) {
        order = serParameters.order;
    }

 protected:

 private:
    bool order;

    // Helper methods for serialization
    size_t calculateTotalElements(const std::vector<uint32_t>& dims);
    void serializeInOrder(const uint8_t* data, size_t elementSize,
                         const std::vector<uint32_t>& dims,
                         std::stringstream& output);
    void serializeReverseOrder(const uint8_t* data, size_t elementSize,
                              const std::vector<uint32_t>& dims,
                              std::stringstream& output);
    void serializeStringsInOrder(const uint8_t* data, size_t dataSize,
                                const std::vector<uint32_t>& dims,
                                std::stringstream& output);
    void serializeStringsReverseOrder(const uint8_t* data, size_t dataSize,
                                     const std::vector<uint32_t>& dims,
                                     std::stringstream& output);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace ser
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_SER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
