/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_LIKELIHOOD_LIKELIHOOD_PARAMETERS_H_
#define SRC_GENIE_LIKELIHOOD_LIKELIHOOD_PARAMETERS_H_

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::likelihood {

// ---------------------------------------------------------------------------------------------------------------------

class LikelihoodParameters {
 private:
    uint8_t num_gl_per_sample;
    bool transform_flag;
    core::DataType dtype_id;

 public:
    LikelihoodParameters();
    LikelihoodParameters(uint8_t _num_gl_per_sample, bool _transform_flag, core::DataType _dtype_id);
    explicit LikelihoodParameters(util::BitReader& reader);

    void read(util::BitReader& reader);
    void Write(genie::util::BitWriter& writer) const;
    [[nodiscard]] size_t GetSize() const;

    uint8_t GetNumGlPerSample() const;
    bool GetTransformFlag() const;
    core::DataType GetDtypeId() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::likelihood

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_LIKELIHOOD_LIKELIHOOD_PARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
