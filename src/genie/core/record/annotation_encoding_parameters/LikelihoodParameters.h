/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_LIKELIHOODPARAMETERS_H_
#define SRC_GENIE_CORE_RECORD_LIKELIHOODPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {

    class LikelihoodParameters {
 private:
    uint8_t num_gl_per_sample;
    bool transform_flag;
    uint8_t dtype_id;

 public:
    LikelihoodParameters();
    LikelihoodParameters(uint8_t num_gl_per_sample, bool transform_flag, uint8_t dtype_id);
    LikelihoodParameters(util::BitReader& reader);
    void read(util::BitReader& reader);
    void write(std::ostream& outputfile) const;
    void write(util::BitWriter& writer) const;

    uint8_t getNumGLPerSample() const { return num_gl_per_sample; }
    bool isTransformFlag() const { return transform_flag; }
    uint8_t getDtypeID() const { return dtype_id; }
};


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_LIKELIHOODPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
