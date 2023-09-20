/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ALGORITHMPARAMETERS_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ALGORITHMPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
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
    std::vector<core::DataType> par_type;
    std::vector<uint8_t> par_num_array_dims;
    std::vector<std::vector<uint8_t>> par_array_dims;
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;


 public:
    AlgorithmParameters();

    explicit AlgorithmParameters(util::BitReader& reader);
    AlgorithmParameters(uint8_t n_pars, std::vector<uint8_t> par_ID, std::vector<core::DataType> par_type,
                        std::vector<uint8_t> par_num_array_dims, std::vector<std::vector<uint8_t>> par_array_dims,
                        std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val);

    void setParameters(std::vector<genie::core::DataType> par_types, std::vector<uint8_t> par_values);

    void read(util::BitReader& reader);
    void write(core::Writer& writer) const;
    size_t getSize(core::Writer& writesize) const;

    uint8_t getNumberOfPars() const { return n_pars; }
    std::vector<uint8_t> getParIDs() const { return par_ID; }
    std::vector<core::DataType> getParTypes() const { return par_type; }
    std::vector<uint8_t> getParNumberOfArrayDims() const { return par_num_array_dims; }
    std::vector<std::vector<uint8_t>> getParArrayDims() const { return par_array_dims; }
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> getParValues() const { return par_val; }
    static std::vector<std::vector<std::vector<std::vector<uint8_t>>>> resizeVector(uint8_t num_array_dims,
                                                                             std::vector<uint8_t> array_dims);
};
template <typename T>
std::vector<std::vector<std::vector<std::vector<uint8_t>>>> parameterToVector(std::vector<T> value, core::DataType type,
                                                                              uint8_t num_array_dims,
                                                                              std::vector<uint8_t> array_dims) {
    std::vector<std::vector<std::vector<std::vector<uint8_t>>>> parameterVector =
        AlgorithmParameters::resizeVector(num_array_dims, array_dims);
    core::ArrayType arrayType;
    auto index = 0;
    for (auto j = 0; j < parameterVector.size(); ++j) {
        for (auto k = 0; k < parameterVector.at(j).size(); ++k) {
            for (auto l = 0; l < parameterVector.at(j).at(k).size(); ++l) {
                parameterVector.at(j).at(k).at(l) = arrayType.toArray(type, value.at(index));
                index++;
            }
        }
    }
    return parameterVector;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ALGORITHMPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
