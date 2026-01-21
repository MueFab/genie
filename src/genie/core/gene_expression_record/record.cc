/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/gene_expression_record/record.h"
#include "genie/core/arrayType.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -------------------------------------------------------------------------------------------------

namespace genie::core::record::gene_expression {

// -------------------------------------------------------------------------------------------------

bool Record::Read(genie::util::BitReader& reader) {
  feature_index = static_cast<uint64_t>(reader.ReadBits(64));
  if (!reader.IsStreamGood())
    return false;
  // Following fields are currently unused, spec <-> input file mismatch
 // feature_name_len = static_cast<uint8_t>(reader.ReadBits(8));
 // if (feature_name_len > 0) {
	//feature_name.resize(feature_name_len);
 //   reader.ReadAlignedBytes(feature_name.data(), feature_name_len);
 // }
  sample_index_from = static_cast<uint32_t>(reader.ReadBits(32));
  sample_count = static_cast<uint32_t>(reader.ReadBits(32));
  expr_attr_count = static_cast<uint8_t>(reader.ReadBits(8));
  expression_attributes.resize(expr_attr_count);
  for (auto i = 0; i < expr_attr_count; ++i) {
    uint8_t attr_len = static_cast<uint8_t>(reader.ReadBits(8));
    std::string attr(attr_len, 0);
    for (auto& c : attr)
      c = reader.ReadAlignedInt<uint8_t>();
    DataType attr_type = static_cast<genie::core::DataType>(reader.ReadAlignedInt<uint8_t>());
    uint8_t array_len = static_cast<uint8_t>(reader.ReadBits(8));
    expression_attributes[i] = ExpressionAttribute(attr, attr_type, array_len);
  }
  ArrayType arrayType;
  for (auto j = 0; j < expr_attr_count; ++j) {
    auto& expr_attr = expression_attributes[j];
    auto expr_attr_values = expr_attr.GetAttrValues();
    expr_attr_values.resize(sample_count);
    for (auto i = 0; i < sample_count; ++i) {
      expr_attr_values[i].resize(expr_attr.GetAttrArrayLen());
      for (auto k = 0; k < expr_attr.GetAttrArrayLen(); ++k) {
        std::vector<uint8_t> value = arrayType.toArray(expr_attr.GetAttrType(), reader);
        expr_attr_values[i][k] = value;
      }
    }
    expr_attr.SetAttrValues(expr_attr_values);
  }
  reader.ReadBits(7);  // reserved
  linked_record = static_cast<uint8_t>(reader.ReadBits(1));
  if (linked_record) {
    link_name_len = static_cast<uint8_t>(reader.ReadBits(8));
    link_name.resize(link_name_len);
    for (auto& c : link_name)
      c = reader.ReadAlignedInt<uint8_t>();
    reference_box_ID = static_cast<uint8_t>(reader.ReadBits(8));
  }
  return true;
}
//-------------------------------------------------------------------------------------------------
void Record::Write(genie::util::BitWriter& writer) {
  (void)writer;
}
// -------------------------------------------------------------------------------------------------
}  // namespace genie::core::record::gene_expression
// -------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
