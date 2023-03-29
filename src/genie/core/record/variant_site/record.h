/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_SITE_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_SITE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include <ostream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

class Info_tag {
 public:
    uint8_t info_tag_len;
    std::string info_tag;
    uint8_t info_type;
    uint8_t info_array_len;
    std::vector<std::string> info_value;
};

class Alt {
 public:
    uint32_t alt_len;
    std::string alt;
};

/**
 *  @brief
 */
class Record {
 private:
    uint64_t variant_index;
    uint16_t seq_ID;
    uint64_t pos;
    uint8_t strand;
    uint8_t ID_len;
    std::string ID;
    uint8_t description_len;
    std::string description;
    uint32_t ref_len;
    std::string ref;
    uint8_t alt_count;

    std::vector<Alt> alt;

    float depth;
    float seq_qual;
    float map_qual;
    float map_num_qual_0;
    uint8_t filters_len;
    std::string filters;
    uint8_t info_count;

    std::vector<Info_tag> info_tag;

    uint8_t linked_record;
    uint8_t link_name_len;
    std::string link_name;
    uint8_t reference_box_ID;

    uint8_t determineSize(uint8_t selectType) const;
 public:
    /**
     * @brief
     */
    Record(genie::util::BitReader& reader);
    
    void write(std::ostream& outputfile) const;

};

class Parse_Record {
 private:
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_SITE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
