/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALGORITHMPARAMETERS_H_
#define SRC_GENIE_CORE_RECORD_ALGORITHMPARAMETERS_H_

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
namespace annotation_parameter_set {

class AlgorithmParameters {
 private:
    uint8_t n_pars;
    std::vector<uint8_t> par_ID;
    std::vector<uint8_t> par_type;
    std::vector<uint8_t> par_num_array_dims;
    std::vector<std::vector<uint8_t>> par_array_dims;
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

 public:
    AlgorithmParameters();
    AlgorithmParameters(util::BitReader& reader);
    AlgorithmParameters(uint8_t n_pars, std::vector<uint8_t> par_ID, std::vector<uint8_t> par_type,
                        std::vector<uint8_t> par_num_array_dims, std::vector<std::vector<uint8_t>> par_array_dims,
                        std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val);

    void read(util::BitReader& reader);
    void write(std::ostream& outputfile) const;
    void write(util::BitWriter& writer) const;

    uint8_t getNumberOfPars() const { return n_pars; }
    std::vector<uint8_t> getParIDs() const { return par_ID; }
    std::vector<uint8_t> getParTypes() const { return par_type; }
    std::vector<uint8_t> getParNumberOfArrayDims() const { return par_num_array_dims; }
    std::vector<std::vector<uint8_t>> getParArrayDims() const { return par_array_dims; }
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> getParValues() const { return par_val; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALGORITHMPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
