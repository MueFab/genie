/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "AttributeParameterSet.h"
#include "genie/core/record/variant_genotype/arrayType.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {


AttributeParameterSet::AttributeParameterSet(const AttributeParameterSet& rec) { *this = rec; }

// ---------------------------------------------------------------------------------------------------------------------



AttributeParameterSet::AttributeParameterSet()
    : attribute_ID(0),
      attribute_name_len(0),
      attribute_name{},
      attribute_type(0),
      attribute_num_array_dims(0),
      attribute_array_dims{},
      attribute_default_val{},
      attribute_miss_val_flag(false),
      attribute_miss_default_flag(false),
      attribute_miss_val{},
      attribute_miss_str{},
      compressor_ID(0),
      n_steps_with_dependencies(0),
      dependency_step_ID{},
      n_dependencies{},
      dependency_var_ID{},
      dependency_is_attribute{},
      dependency_ID{} {}

AttributeParameterSet::AttributeParameterSet(
    uint16_t attribute_ID, uint8_t attribute_name_len, std::string attribute_name, uint8_t attribute_type,
    uint8_t attribute_num_array_dims, std::vector<uint8_t> attribute_array_dims,
    std::vector<uint8_t> attribute_default_val, bool attribute_miss_val_flag, bool attribute_miss_default_flag,
    std::vector<uint8_t> attribute_miss_val, std::string attribute_miss_str, uint8_t compressor_ID,
    uint8_t n_steps_with_dependencies, std::vector<uint8_t> dependency_step_ID, std::vector<uint8_t> n_dependencies,
    std::vector<std::vector<uint8_t>> dependency_var_ID, std::vector<std::vector<bool>> dependency_is_attribute,
    std::vector<std::vector<uint16_t>> dependency_ID)
    : attribute_ID(attribute_ID),
      attribute_name_len(attribute_name_len),
      attribute_name(attribute_name),
      attribute_type(attribute_type),
      attribute_num_array_dims(attribute_num_array_dims),
      attribute_array_dims(attribute_array_dims),
      attribute_default_val(attribute_default_val),
      attribute_miss_val_flag(attribute_miss_val_flag),
      attribute_miss_default_flag(attribute_miss_default_flag),
      attribute_miss_val(attribute_miss_val),
      attribute_miss_str(attribute_miss_str),
      compressor_ID(compressor_ID),
      n_steps_with_dependencies(n_steps_with_dependencies),
      dependency_step_ID(dependency_step_ID),
      n_dependencies(n_dependencies),
      dependency_var_ID(dependency_var_ID),
      dependency_is_attribute(dependency_is_attribute),
      dependency_ID(dependency_ID) {}


AttributeParameterSet& AttributeParameterSet::operator=(const AttributeParameterSet& rec) {
    if (this == &rec) {
        return *this;
    }
    this->attribute_ID = rec.attribute_ID;
    this->attribute_name_len = rec.attribute_name_len;
    this->attribute_name = rec.attribute_name;
    this->attribute_type = rec.attribute_type;
    this->attribute_num_array_dims = rec.attribute_num_array_dims;
    this->attribute_array_dims = rec.attribute_array_dims;
    this->attribute_default_val = rec.attribute_default_val;
    this->attribute_miss_val_flag = rec.attribute_miss_val_flag;
    this->attribute_miss_default_flag = rec.attribute_miss_default_flag;
    this->attribute_miss_val = rec.attribute_miss_val;
    this->attribute_miss_str = rec.attribute_miss_str;
    this->compressor_ID = rec.compressor_ID;
    this->n_steps_with_dependencies = rec.n_steps_with_dependencies;
    this->dependency_step_ID = rec.dependency_step_ID;
    this->n_dependencies = rec.n_dependencies;
    this->dependency_var_ID = rec.dependency_var_ID;
    this->dependency_is_attribute = rec.dependency_is_attribute;
    this->dependency_ID = rec.dependency_ID;

    return *this;
}



AttributeParameterSet::AttributeParameterSet(util::BitReader& reader) { read(reader); }

void AttributeParameterSet::read(util::BitReader& reader) {
    attribute_ID = reader.readBypassBE<uint16_t>();
    attribute_name_len = reader.readBypassBE<uint8_t>();
    attribute_name.resize(attribute_name_len);
    reader.readBypass(attribute_name);
    attribute_type = reader.readBypassBE<uint8_t>();
    attribute_num_array_dims = static_cast<uint8_t>(reader.read_b(2));
    for (auto i = 0; i < attribute_num_array_dims; ++i)
        attribute_array_dims.push_back(static_cast<uint8_t>(reader.read_b(8)));

    variant_genotype::arrayType curType;
    attribute_default_val = curType.toArray(attribute_type, reader);
    attribute_miss_val_flag = static_cast<bool>(reader.read_b(1));
    if (attribute_miss_val_flag) {
        attribute_miss_default_flag = static_cast<bool>(reader.read_b(1));
        if (!attribute_miss_default_flag) attribute_miss_val = curType.toArray(attribute_type, reader);
        reader.readBypass_null_terminated(attribute_miss_str);
    }
    compressor_ID = static_cast<uint8_t>(reader.read_b(8));

    n_steps_with_dependencies = static_cast<uint8_t>(reader.read_b(4));
    dependency_var_ID.resize(n_steps_with_dependencies);
    dependency_is_attribute.resize(n_steps_with_dependencies);

    for (auto i = 0; i < n_steps_with_dependencies; ++i) {
        dependency_step_ID.push_back(static_cast<uint8_t>(reader.read_b(4)));
        n_dependencies.push_back(static_cast<uint8_t>(reader.read_b(4)));
        for (auto j = 0; j < n_dependencies.back(); ++j) {
            dependency_var_ID[i].push_back(static_cast<uint8_t>(reader.read_b(4)));
            dependency_is_attribute[i].push_back(static_cast<bool>(reader.read_b(1)));
            if (dependency_is_attribute[i].back())
                dependency_ID[i].push_back(static_cast<uint16_t>(reader.read_b(16)));
            else
                dependency_ID[i].push_back(static_cast<uint16_t>(reader.read_b(7)));
        }
    }
}

void AttributeParameterSet::write(util::BitWriter& writer) const {
    writer.write(attribute_ID, 16);
    writer.write(attribute_name_len, 8);
    writer.write(attribute_name);
    writer.write(attribute_type, 8);
    writer.write(attribute_num_array_dims, 2);
    for (auto attribute_dim : attribute_array_dims) writer.write(attribute_dim, 8);

    for (auto i = attribute_default_val.size(); i > 0; --i) writer.write(attribute_default_val[i-1], 8);

    writer.write(attribute_miss_val_flag, 1);
    if (attribute_miss_val_flag) {
        writer.write(attribute_miss_default_flag, 1);
        if (!attribute_miss_default_flag)
            for (auto byte : attribute_miss_val) writer.write(byte, 8);
    }
    writer.write(attribute_miss_str);
    writer.write(0, 8);

    writer.write(compressor_ID, 8);
    writer.write(n_steps_with_dependencies, 4);
    for (auto i = 0; i < n_steps_with_dependencies; ++i) {
        writer.write(dependency_step_ID[i], 4);
        writer.write(n_dependencies[i], 4);
        for (auto j = 0; j < n_dependencies[i]; ++j) {
            writer.write(dependency_var_ID[i][j], 4);
            writer.write(dependency_is_attribute[i][j], 1);
            if (dependency_is_attribute[i][j])
                writer.write(dependency_ID[i][j], 16);
            else
                writer.write(dependency_ID[i][j], 7);
        }
    }
    writer.flush();
}

void AttributeParameterSet::write(std::ostream& outputfile) const {
    outputfile << std::to_string(attribute_ID) << ",";
    outputfile << std::to_string(attribute_name_len) << ",";
    outputfile << '"' << attribute_name << '"' << ",";
    outputfile << std::to_string(attribute_type) << ",";
    outputfile << std::to_string(attribute_num_array_dims) << ",";

    for (auto array_dims : attribute_array_dims) {
        outputfile << std::to_string(array_dims )<< ",";
    }

    variant_genotype::arrayType curType;
    outputfile << curType.toString(attribute_type, attribute_default_val) << ",";
    outputfile << std::to_string(attribute_miss_val_flag) << ",";
    if (attribute_miss_val_flag) {
        outputfile << std::to_string(attribute_miss_default_flag) << ",";
        if (!attribute_miss_default_flag) outputfile << curType.toString(attribute_type, attribute_miss_val) << ",";
        outputfile << '"' << (attribute_miss_str) << '"' << ",";
    }
    outputfile << std::to_string(compressor_ID )<< ",";
    outputfile << std::to_string(n_steps_with_dependencies )<< ",";
    for (auto i = 0; i < n_steps_with_dependencies; ++i) {
        outputfile << std::to_string(dependency_step_ID[i]) << ",";
        outputfile << std::to_string(n_dependencies[i]) << ",";
        for (auto j = 0; j < n_dependencies[i]; ++j) {
            outputfile << std::to_string(dependency_var_ID[i][j]) << ",";
            outputfile << std::to_string(dependency_is_attribute[i][j]) << ",";
            outputfile << std::to_string(dependency_ID[i][j]) << ",";
        }
    }
}
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------