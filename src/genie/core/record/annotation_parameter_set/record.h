/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_RECORD_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <sstream>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "genie/core/record/annotation_parameter_set/AnnotationEncodingParameters.h"
#include "genie/core/record/annotation_parameter_set/TileConfiguration.h"
#include "genie/core/writer.h"
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
    core::AlphabetID AT_alphabet_ID;
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
    Record(uint8_t parameter_set_ID, uint8_t AT_ID, core::AlphabetID AT_alphabet_ID, uint8_t AT_coord_size,
           bool AT_pos_40_bits_flag, uint8_t n_aux_attribute_groups, std::vector<TileConfiguration> tile_configuration,
           AnnotationEncodingParameters annotation_encoding_parameters);

    void read(util::BitReader& reader);
    void write(Writer& writer) const;
    void write(util::BitWriter& writer) const;

    size_t getSize() const;
    size_t getSize(core::Writer& writesize) const;

    uint8_t getParameterSetID() const { return parameter_set_ID; }
    uint8_t getATID() const { return AT_ID; }
    core::AlphabetID getATAlphbetID() const { return AT_alphabet_ID; }
    uint8_t getATCoordSize() const { return AT_coord_size; }
    bool isATPos40Bits() const { return AT_pos_40_bits_flag; }
    uint8_t getNumberOfAuxAttributeGroups() const { return n_aux_attribute_groups; }
    std::vector<TileConfiguration> getTileConfigurations() const { return tile_configuration; }
    AnnotationEncodingParameters getAnnotationEncodingParameters() const { return annotation_encoding_parameters; }
};

struct ParameterSettings {
    uint8_t parameter_set_ID;
    uint8_t AT_ID;
    uint8_t AG_class;
    core::AlphabetID AT_alphabet_ID;
    uint8_t ATCoordSize;
    bool AT_pos_40_bits_flag;
    uint8_t n_aux_attribute_groups;
    TileParameterSettings tileParameterSettings;

    ParameterSettings()
        : parameter_set_ID(1),
          AT_ID(0),
        AG_class(0),
          AT_alphabet_ID(core::AlphabetID::ACGTN),
          ATCoordSize(3),
          AT_pos_40_bits_flag(false),
          n_aux_attribute_groups(0),
          tileParameterSettings() {}
};
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
