/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_DATAUNIT_H_
#define SRC_GENIE_CORE_RECORD_DATAUNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::data_unit {

/**
 * @brief Class representing raw reference data
 */
class RawReference {
 public:
  /**
   * @brief Gets the size of the raw reference
   * @return Size of the raw reference
   */
  [[nodiscard]] static size_t GetSize() {
    return 0;
  }
};

/**
 * @brief Class representing parameter set data
 */
class ParameterSet {
 public:
  /**
   * @brief Gets the size of the parameter set
   * @return Size of the parameter set
   */
  [[nodiscard]] static size_t GetSize() {
    return 0;
  }
};

/**
 * @brief Class representing annotation access unit data
 */
class AccessUnit {
 public:
  /**
   * @brief Gets the size of the access unit
   * @return Size of the access unit
   */
  [[nodiscard]] static size_t GetSize() {
    return 0;
  }
};

/**
 * @brief Class representing annotation access unit data
 */
class AnnotationAccessUnit {
 public:
  /**
   * @brief Gets the size of the annotation access unit
   * @return Size of the annotation access unit
   */
  [[nodiscard]] static size_t GetSize() {
    return 0;
  }
};

/**
 * @brief Class representing a data unit record
 *
 * A record can contain one of several types of data: raw reference,
 * parameter set, access unit, annotation access unit, or annotation parameter set.
 */
class Record {
 private:
  uint8_t data_unit_type_;
  uint64_t data_unit_size_;  // TODO: Compute on-the-fly (?)
  RawReference rawReference_;
  ParameterSet parameterSet_;
  AccessUnit access_unit_;
  genie::core::record::annotation_access_unit::Record annotation_access_unit_;
  genie::core::record::annotation_parameter_set::Record annotation_parameter_set_;

 public:
  /**
   * @brief Default constructor
   */
  Record();

  /**
   * @brief Constructs a record with raw reference data
   * @param raw_reference The raw reference data
   */
  explicit Record(RawReference& raw_reference);

  /**
   * @brief Constructs a record with parameter set data
   * @param parameter_set The parameter set data
   */
  explicit Record(ParameterSet& parameter_set);

  /**
   * @brief Constructs a record with access unit data
   * @param access_unit The access unit data
   */
  explicit Record(AccessUnit& access_unit);

  /**
   * @brief Constructs a record with annotation access unit data
   * @param annotation_access_unit The annotation access unit data
   */
  explicit Record(annotation_access_unit::Record& annotation_access_unit);

  /**
   * @brief Constructs a record with annotation parameter set data
   * @param annotation_parameter_set The annotation parameter set data
   */
  explicit Record(annotation_parameter_set::Record& annotation_parameter_set);

  /**
   * @brief Writes the complete record to a writer
   * @param writer The writer to write to
   * @return Number of bytes written
   */
  uint64_t write(core::Writer& writer) const;

  /**
   * @brief Writes a specific amount of the record to a writer
   * @param writer The writer to write to
   * @param write_size The number of bytes to write
   */
  void write(core::Writer& writer, uint64_t write_size) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::data_unit

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_DATAUNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
