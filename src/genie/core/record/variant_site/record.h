/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_SITE_RECORD_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_SITE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

class Info_tag {
 public:
    uint8_t info_tag_len{};
    std::string info_tag{};
    uint8_t info_type{};
    uint8_t info_array_len{};
    std::vector<std::string> info_value{};
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

    std::vector <uint32_t> alt_len;
    std::vector <std::string> altern;

    uint32_t depth;
    uint32_t seq_qual;
    uint32_t map_qual;
    uint32_t map_num_qual_0;
    uint8_t filters_len;
    std::string filters;
    uint8_t info_count;

    std::vector<Info_tag> info_tag;

    uint8_t linked_record;
    uint8_t link_name_len;
    std::string link_name;
    uint8_t reference_box_ID;

    uint8_t determineSize(uint8_t selectType) const;
    std::string infoToCorrectString(std::string value, uint8_t selectType) const;

 public:
    /**
     * @brief
     */
    Record()
        : variant_index(0),
          seq_ID(0),
          pos(0),
          strand(0),
          ID_len(0),
          ID(""),
          description_len(0),
          description(""),
          ref_len(0),
          ref(""),
          alt_count(0),
          alt_len(0),
          altern(0),
          depth(0),
          seq_qual(0),
          map_qual(0),
          map_num_qual_0(0),
          filters_len(0),
          filters(""),
          info_count(0),
          info_tag(0),
          linked_record(0),
          link_name_len(0),
          link_name(""),
          reference_box_ID(0) {}
    /**
     * @brief
     */
    explicit Record(genie::util::BitReader& reader);
    /**
     * @brief
     */
    bool read(genie::util::BitReader& reader);
    /**
     * @brief
     */
    void write(std::ostream& outputfile) const;

    uint64_t getVariantIndex() { return variant_index; }
    uint16_t getSeqID() { return seq_ID; }
    uint64_t getPos() { return pos; }
    uint8_t getStrand() { return strand; }
    std::string getID() { return ID; }
    std::string getDescription() { return description; }
    std::string getRef() { return ref; }
    uint8_t getAltCount() { return alt_count; }

    std::vector<std::string> getAlt() { return altern; }
    uint32_t getDepth() { return depth; }
    uint32_t getSeqQual() { return seq_qual; }
    uint32_t getMapQual() { return map_qual; }
    uint32_t getMapNumQual0() { return map_num_qual_0; }
    std::string getFilters() { return filters; }
    uint8_t getInfoCount() { return info_count; }
    std::vector<Info_tag> getInfoTag() { return info_tag; }
    bool isLinkedRecord() { return (linked_record == 0 ? false : true); }
    std::string getLinkName() { return link_name; }
    uint8_t getReferenceBoxID() { return reference_box_ID; }
};


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_SITE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
