/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATIONENPARAMETERSET_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATIONENPARAMETERSET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "AnnotationEncodingParameters.h"
#include "TileConfiguration.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {


/**
 *  @brief
 */
class Record {
 private:
    uint8_t parameter_set_ID;
    uint8_t AT_ID;
    uint8_t AT_aphabet_ID;
    uint8_t AT_coord_size;
    bool AT_pos_40_bits_flag;
    uint8_t n_aux_attribute_groups;
    std::vector<TileConfiguration> tile_configuration;
    AnnotationEncodingParameters annotation_encoding_parameters;
 public:
    /**
     * @brief
     */
    Record();
    void read(util::BitReader& reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATIONENPARAMETERSET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
