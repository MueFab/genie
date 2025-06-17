/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <algorithm>
#include <filesystem>
#include <string>
#include <utility>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

#include "genie/core/data_unit_record/record.h"

// -------------------------------------------------------------------------------------------------

namespace genie::core::record::data_unit {

// -------------------------------------------------------------------------------------------------

Record::Record() : data_unit_type_(0), data_unit_size_(0) {}

// -------------------------------------------------------------------------------------------------

Record::Record(RawReference& _rawReference)
    : data_unit_type_(0), data_unit_size_(0), rawReference_(_rawReference) {}

// -------------------------------------------------------------------------------------------------

Record::Record(ParameterSet& _parameterSet) : data_unit_type_(1), parameterSet_(_parameterSet) {
  data_unit_size_ = this->parameterSet_.GetSize();
}

// -------------------------------------------------------------------------------------------------

Record::Record(AccessUnit& _accessUnit)
    : data_unit_type_(2), data_unit_size_(0), access_unit_(_accessUnit) {}

// -------------------------------------------------------------------------------------------------

Record::Record(annotation_parameter_set::Record& _annotationParameterSet)
    : data_unit_type_(3), data_unit_size_(0), annotation_parameter_set_(_annotationParameterSet) {}

// -------------------------------------------------------------------------------------------------

Record::Record(annotation_access_unit::Record& _annotationAccessUnit)
    : data_unit_type_(4), data_unit_size_(0) {
  annotation_access_unit_ = _annotationAccessUnit;
}

// -------------------------------------------------------------------------------------------------

void Record::Write(util::BitWriter& writer, uint64_t write_size) const {
  writer.WriteBits(data_unit_type_, 8);
  switch (data_unit_type_) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      writer.WriteReserved(10);
      writer.WriteBits(write_size, 22);
      annotation_parameter_set_.write(writer);
      break;
    case 4:
      writer.WriteReserved(3);
      writer.WriteBits(write_size, 29);
      annotation_access_unit_.write(writer);
      break;
    default:
      break;
  }
}

// -------------------------------------------------------------------------------------------------

uint64_t genie::core::record::data_unit::Record::Write(util::BitWriter& writer) const {
  writer.WriteBits(data_unit_type_, 8);
  uint64_t writesize = 0;
  switch (data_unit_type_) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      writer.WriteReserved(10);
      writesize = (annotation_parameter_set_.getSize() + 40) / 8;
      writer.WriteBits(writesize, 22);
      annotation_parameter_set_.write(writer);
      break;
    case 4:
      writer.WriteReserved(3);
      writesize = (annotation_access_unit_.getSize() + 40) / 8;
      writer.WriteBits(writesize, 29);
      annotation_access_unit_.write(writer);
      break;
    default:
      break;
  }
  return writesize;
}

// -------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::data_unit

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
