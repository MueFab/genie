/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ATTRIBUTEPARAMETERSET_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ATTRIBUTEPARAMETERSET_H_

// ---------------------------------------------------------------------------------------------------------------------

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
namespace core {
namespace record {
namespace annotation_parameter_set {

class AttributeParameterSet {
 private:
    uint16_t attribute_ID;
    uint8_t attribute_name_len;
    std::string attribute_name;
    core::DataType attribute_type;
    uint8_t attribute_num_array_dims;
    std::vector<uint8_t> attribute_array_dims;

    std::vector<uint8_t> attribute_default_val;
    bool attribute_miss_val_flag;
    bool attribute_miss_default_flag;
    std::vector<uint8_t> attribute_miss_val;

    std::string attribute_miss_str;
    uint8_t compressor_ID;

    uint8_t n_steps_with_dependencies;
    std::vector<uint8_t> dependency_step_ID;
    std::vector<uint8_t> n_dependencies;
    std::vector<std::vector<uint8_t>> dependency_var_ID;
    std::vector<std::vector<bool>> dependency_is_attribute;
    std::vector<std::vector<uint16_t>> dependency_ID;

 public:
    AttributeParameterSet();
    AttributeParameterSet(uint16_t attribute_ID,  std::string attribute_name,
                          core::DataType attribute_type,
                          std::vector<uint8_t> attribute_array_dims, std::vector<uint8_t> attribute_default_val,
                          bool attribute_miss_val_flag, bool attribute_miss_default_flag,
                          std::vector<uint8_t> attribute_miss_val, std::string attribute_miss_str,
                          uint8_t compressor_ID)
        : AttributeParameterSet(attribute_ID, (uint8_t)attribute_name.size(), attribute_name, attribute_type,
                                (uint8_t)attribute_array_dims.size(), attribute_array_dims, attribute_default_val,
                                attribute_miss_val_flag, attribute_miss_default_flag, attribute_miss_val,
                                attribute_miss_str, compressor_ID, 0u, {}, {}, {{}}, {{}}, {{}})
    {}


    AttributeParameterSet(uint16_t attribute_ID, uint8_t attribute_name_len, std::string attribute_name,
                          core::DataType attribute_type, uint8_t attribute_num_array_dims,
                          std::vector<uint8_t> attribute_array_dims, std::vector<uint8_t> attribute_default_val,
                          bool attribute_miss_val_flag, bool attribute_miss_default_flag,
                          std::vector<uint8_t> attribute_miss_val, std::string attribute_miss_str,
                          uint8_t compressor_ID, uint8_t n_steps_with_dependencies,
                          std::vector<uint8_t> dependency_step_ID, std::vector<uint8_t> n_dependencies,
                          std::vector<std::vector<uint8_t>> dependency_var_ID,
                          std::vector<std::vector<bool>> dependency_is_attribute,
                          std::vector<std::vector<uint16_t>> dependency_ID);
    /**
     * @brief
     * @param reader
     */
    explicit AttributeParameterSet(util::BitReader& reader);

    /**
     * @brief
     * @param rec
     */
    AttributeParameterSet(const AttributeParameterSet& rec);

    /**
     * @brief
     * @param rec
     */
    AttributeParameterSet& operator=(const AttributeParameterSet& rec);

    /**
     * @brief
     * @param reader
     */
    void read(util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(core::Writer& writer) const;

    size_t getSize(core::Writer& writesize) const;

    uint16_t getAttriubuteID() const { return attribute_ID; }
    uint8_t getAttributeNameLength() const { return attribute_name_len; }
    std::string getAttributeName() const { return attribute_name; }
    core::DataType getAttributeType() const { return attribute_type; }
    uint8_t getAttributeNumberOFArrayDims() const { return attribute_num_array_dims; }
    std::vector<uint8_t> getAttributeArrayDims() const { return attribute_array_dims; }

    std::vector<uint8_t> getAttributeDefaultValue() const { return attribute_default_val; }
    bool isAttributeMissedValue() const { return attribute_miss_val_flag; }
    bool isAttributeMissedDefault() const { return attribute_miss_default_flag; }
    std::vector<uint8_t> getAttributeMissedValues() const { return attribute_miss_val; }

    std::string getAttributeMissedString() const { return attribute_miss_str; }
    uint8_t getCompressorID() const { return compressor_ID; }

    uint8_t getNumberOfStepsWithDependencies() const { return n_steps_with_dependencies; }
    std::vector<uint8_t> GetDependencyStepIDs() const { return dependency_step_ID; }
    std::vector<uint8_t> GetNumberOfDependencies() const { return n_dependencies; }
    std::vector<std::vector<uint8_t>> getDependencyVarIDs() const { return dependency_var_ID; }
    std::vector<std::vector<bool>> areDependeciesAttributes() const { return dependency_is_attribute; }
    std::vector<std::vector<uint16_t>> getDependencyIDs() const { return dependency_ID; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ATTRIBUTEPARAMETERSET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
