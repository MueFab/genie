/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_PARAMETERS_H
#define GENIE_LIKELIHOOD_PARAMETERS_H

#include <boost/optional/optional.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// ---------------------------------------------------------------------------------------------------------------------

class LikelihoodParameters {
 private:
    uint8_t num_gl_per_sample;
    bool transform_flag;
    core::DataType dtype_id;

 public:
    LikelihoodParameters();
    LikelihoodParameters(uint8_t _num_gl_per_sample, bool _transform_flag, core::DataType _dtype_id);

    void write(core::Writer& writer) const;
    void write(genie::util::BitWriter& writer) const;
    void read(util::BitReader& reader);
    size_t getSize(core::Writer& writesize) const;

    uint8_t getNumGlPerSample() const;
    bool getTransformFlag() const;
    core::DataType getDtypeID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace likelihood
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_LIKELIHOOD_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
