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

#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/core/access_unit/annotation/typed_data.h"
#include "genie/core/constants.h"
#include "genie/entropy/base/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::ser {

class SERParameters {
 public:
    bool order;

    SERParameters()
        : order(true) {
    }

    explicit SERParameters(bool _order)
        : order(_order) {
    }

    core::parameter::annotation::AlgorithmParameters convertToAlgorithmParameters() const;

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

}  // namespace genie::entropy::ser

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_SER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
