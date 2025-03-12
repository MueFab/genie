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
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "AttributeParameterSet.h"
#include "genie/core/arrayType.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

AttributeParameterSet::AttributeParameterSet(const AttributeParameterSet& rec) { *this = rec; }

// ---------------------------------------------------------------------------------------------------------------------

AttributeParameterSet::AttributeParameterSet()
    : attribute_ID(2),
      attribute_name_len(0),
      attribute_name{},
      attribute_type(DataType::STRING),
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
    uint16_t _attribute_ID, uint8_t _attribute_name_len, std::string _attribute_name, core::DataType _attribute_type,
    uint8_t _attribute_num_array_dims, std::vector<uint8_t> _attribute_array_dims,
    std::vector<uint8_t> attribute_default_val, bool _attribute_miss_val_flag, bool _attribute_miss_default_flag,
    std::vector<uint8_t> _attribute_miss_val, std::string _attribute_miss_str, uint8_t _compressor_ID,
    uint8_t _n_steps_with_dependencies, std::vector<uint8_t> _dependency_step_ID, std::vector<uint8_t> _n_dependencies,
    std::vector<std::vector<uint8_t>> _dependency_var_ID, std::vector<std::vector<bool>> _dependency_is_attribute,
    std::vector<std::vector<uint16_t>> _dependency_ID)
    : attribute_ID(_attribute_ID),
      attribute_name_len(_attribute_name_len),
      attribute_name(_attribute_name),
      attribute_type(_attribute_type),
      attribute_num_array_dims(_attribute_num_array_dims),
      attribute_array_dims(_attribute_array_dims),
      attribute_default_val(attribute_default_val),
      attribute_miss_val_flag(_attribute_miss_val_flag),
      attribute_miss_default_flag(_attribute_miss_default_flag),
      attribute_miss_val(_attribute_miss_val),
      attribute_miss_str(_attribute_miss_str),
      compressor_ID(_compressor_ID),
      n_steps_with_dependencies(_n_steps_with_dependencies),
      dependency_step_ID(_dependency_step_ID),
      n_dependencies(_n_dependencies),
      dependency_var_ID(_dependency_var_ID),
      dependency_is_attribute(_dependency_is_attribute),
      dependency_ID(_dependency_ID) {
    if (!attribute_miss_val_flag) {
        this->attribute_miss_default_flag = false;
        this->attribute_miss_str = "";
    }
    if (attribute_miss_default_flag) this->attribute_miss_val.clear();
}

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
    attribute_ID = static_cast<uint16_t>(reader.ReadBits(16));
    attribute_name_len = static_cast<uint8_t>(reader.ReadBits(8));
    attribute_name.resize(attribute_name_len);
    for (auto& attribute_nameChar : attribute_name) attribute_nameChar = static_cast<char>(reader.ReadBits(8));

    attribute_type = static_cast<DataType>(reader.ReadBits(8));
    attribute_num_array_dims = static_cast<uint8_t>(reader.ReadBits(2));
    attribute_array_dims.resize(attribute_num_array_dims);
    for (auto i = 0; i < attribute_num_array_dims; ++i)
        attribute_array_dims[i] = static_cast<uint8_t>(reader.ReadBits(8));

    ArrayType curType;
    attribute_default_val = curType.toArray(attribute_type, reader);
    attribute_miss_val_flag = static_cast<bool>(reader.ReadBits(1));
    if (attribute_miss_val_flag) {
        attribute_miss_default_flag = static_cast<bool>(reader.ReadBits(1));
        if (!attribute_miss_default_flag) attribute_miss_val = curType.toArray(attribute_type, reader);

        char readChar = 0;
        do {
            readChar = static_cast<char>(reader.ReadBits(8));
            if (readChar != 0) attribute_miss_str += readChar;
        } while (readChar != 0);
    }
    compressor_ID = static_cast<uint8_t>(reader.ReadBits(8));

    n_steps_with_dependencies = static_cast<uint8_t>(reader.ReadBits(4));

    n_dependencies.resize(n_steps_with_dependencies);
    dependency_step_ID.resize(n_steps_with_dependencies);
    dependency_var_ID.resize(n_steps_with_dependencies);
    dependency_is_attribute.resize(n_steps_with_dependencies);
    dependency_ID.resize(n_steps_with_dependencies);

    for (auto i = 0; i < n_steps_with_dependencies; ++i) {
        dependency_step_ID[i] = static_cast<uint8_t>(reader.ReadBits(4));
        n_dependencies[i] = static_cast<uint8_t>(reader.ReadBits(4));

        dependency_var_ID[i].resize(n_dependencies[i]);
        dependency_is_attribute[i].resize(n_dependencies[i]);
        dependency_ID[i].resize(n_dependencies[i]);

        for (auto j = 0; j < n_dependencies[i]; ++j) {
            dependency_var_ID[i][j] = static_cast<uint8_t>(reader.ReadBits(4));
            dependency_is_attribute[i][j] = static_cast<bool>(reader.ReadBits(1));
            if (dependency_is_attribute[i][j])
                dependency_ID[i][j] = static_cast<uint16_t>(reader.ReadBits(16));
            else
                dependency_ID[i][j] = static_cast<uint16_t>(reader.ReadBits(7));
        }
    }
    reader.FlushHeldBits();
}

void AttributeParameterSet::write(core::Writer& writer) const {
    ArrayType curType;
    writer.Write(attribute_ID, 16);
    writer.Write(attribute_name_len, 8);
    writer.Write(attribute_name);
    writer.Write(static_cast<uint8_t>(attribute_type), 8);
    writer.Write(attribute_num_array_dims, 2);
    for (auto attribute_dim : attribute_array_dims)
      writer.Write(attribute_dim, 8);
    curType.toFile(attribute_type, attribute_default_val, writer);

    writer.Write(attribute_miss_val_flag, 1);
    if (attribute_miss_val_flag) {
      writer.Write(attribute_miss_default_flag, 1);
        if (!attribute_miss_default_flag) curType.toFile(attribute_type, attribute_miss_val, writer);
        writer.Write(attribute_miss_str);
        writer.WriteReserved(8);
    }

    writer.Write(compressor_ID, 8);
    writer.Write(n_steps_with_dependencies, 4);
    for (auto i = 0; i < n_steps_with_dependencies; ++i) {
      writer.Write(dependency_step_ID[i], 4);
        writer.Write(n_dependencies[i], 4);
        for (auto j = 0; j < n_dependencies[i]; ++j) {
          writer.Write(dependency_var_ID[i][j], 4);
            writer.Write(dependency_is_attribute[i][j], 1);
            if (dependency_is_attribute[i][j])
              writer.Write(dependency_ID[i][j], 16);
            else
              writer.Write(dependency_ID[i][j], 7);
        }
    }
    writer.Flush();
}

void AttributeParameterSet::write(util::BitWriter& writer) const {
  ArrayType curType;
  writer.WriteBits(attribute_ID, 16);
  writer.WriteBits(attribute_name_len, 8);
  writer.Write(attribute_name);
  writer.WriteBits(static_cast<uint8_t>(attribute_type), 8);
  writer.WriteBits(attribute_num_array_dims, 2);
  for (auto attribute_dim : attribute_array_dims)
    writer.WriteBits(attribute_dim, 8);
  curType.toFile(attribute_type, attribute_default_val, writer);

  writer.WriteBits(attribute_miss_val_flag, 1);
  if (attribute_miss_val_flag) {
    writer.WriteBits(attribute_miss_default_flag, 1);
    if (!attribute_miss_default_flag) curType.toFile(attribute_type, attribute_miss_val, writer);
    writer.Write(attribute_miss_str);
    writer.WriteReserved(8);
  }

  writer.WriteBits(compressor_ID, 8);
  writer.WriteBits(n_steps_with_dependencies, 4);
  for (auto i = 0; i < n_steps_with_dependencies; ++i) {
    writer.WriteBits(dependency_step_ID[i], 4);
    writer.WriteBits(n_dependencies[i], 4);
    for (auto j = 0; j < n_dependencies[i]; ++j) {
      writer.WriteBits(dependency_var_ID[i][j], 4);
      writer.WriteBits(dependency_is_attribute[i][j], 1);
      if (dependency_is_attribute[i][j])
        writer.WriteBits(dependency_ID[i][j], 16);
      else
        writer.WriteBits(dependency_ID[i][j], 7);
    }
  }
  writer.FlushBits();
}

size_t AttributeParameterSet::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.GetBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
