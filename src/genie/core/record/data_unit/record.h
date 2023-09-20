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
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace data_unit {

class RawReference {
 public:
    size_t getSize() const { return 0; }
};
class ParameterSet {
 public:
    size_t getSize() const { return 0; }
};
class AccessUnit {
 public:
    size_t getSize() const { return 0; }
};
class AnnotationAccessUnit {
 public:
    size_t getSize() const { return 0; }
};

/**
 *  @brief
 */
class Record {
 private:
    uint8_t data_unit_type;
    uint64_t data_unit_size; // TODO: Compute on-the-fly (?)
    RawReference rawReference;
    ParameterSet parameterSet;
    AccessUnit accessUnit;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet;


 public:
    /**
     * @brief
     */
    Record();

    explicit Record(RawReference rawReference);
    explicit Record(ParameterSet parameterSet);
    explicit Record(AccessUnit accessUnit);

    explicit Record(annotation_access_unit::Record annotationAccessUnit);
    explicit Record(annotation_parameter_set::Record annotationParameterSet);

    void write(core::Writer& writer) const;

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace data_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_DATAUNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
