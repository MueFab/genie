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
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/arrayType.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

class AttributeData {
 public:
    AttributeData();
    AttributeData(uint8_t length, std::string name, uint16_t attributeID);
    AttributeData(uint8_t length, std::string name, genie::core::DataType type, uint8_t arrayLength, uint16_t attributeID);

    AttributeData& operator=(const AttributeData& other);

    AttributeData(const AttributeData& other);

    void setAttributeType(genie::core::DataType value) { attributeType = value; }
    void setArrayLength(uint8_t value) { attributeArrayDims = value; }

    uint8_t getAttributeNameLength() const { return attributeNameLength; }
    std::string getAttributeName() const { return attributeName; }
    genie::core::DataType getAttributeType() const { return attributeType; }
    uint8_t getArrayLength() const { return attributeArrayDims; }
    uint16_t getAttributeID() const { return attributeID; }

 private:
    uint16_t attributeID;
    uint8_t attributeNameLength;
    std::string attributeName;
    genie::core::DataType attributeType;
    uint8_t attributeArrayDims;
};

class Info_tag {
 public:
    uint8_t info_tag_len{};
    std::string info_tag{};
    genie::core::DataType info_type{};
    uint8_t info_array_len{};
    std::vector<std::vector<uint8_t>> infoValue;
    Info_tag& operator=(const Info_tag& other) {
        info_tag_len = other.info_tag_len;
        info_tag = other.info_tag;
        info_type = other.info_type;
        info_array_len = other.info_array_len;
        infoValue = other.infoValue;
        return *this;
    }
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

    std::vector<uint32_t> alt_len;
    std::vector<std::string> altern;

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
    std::string infoToCorrectString(std::string& value, uint8_t selectType) const;
    void clearData();

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
    Record(uint64_t _variant_index,
           uint16_t _seq_ID,
           uint64_t _pos,
           uint8_t _strand,
           uint8_t _ID_len,
           std::string& _ID,
           uint8_t _description_len,
           std::string _description,
           uint32_t _ref_len,
           std::string _ref,
           uint8_t _alt_count,
           std::vector<uint32_t> _alt_len,
           std::vector<std::string> _altern,
           uint32_t _depth,
           uint32_t _seq_qual,
           uint32_t _map_qual,
           uint32_t _map_num_qual_0,
           uint8_t _filters_len,
           std::string _filters,
           uint8_t _info_count,
           std::vector<Info_tag> _info_tag,
           uint8_t _linked_record,
           uint8_t _link_name_len,
           std::string _link_name,
           uint8_t _reference_box_ID)
        : variant_index(_variant_index),
          seq_ID(_seq_ID),
          pos(_pos),
          strand(_strand),
          ID_len(_ID_len),
          ID(_ID),
          description_len(_description_len),
          description(_description),
          ref_len(_ref_len),
          ref(_ref),
          alt_count(_alt_count),
          alt_len(_alt_len),
          altern(_altern),
          depth(_depth),
          seq_qual(_seq_qual),
          map_qual(_map_qual),
          map_num_qual_0(_map_num_qual_0),
          filters_len(_filters_len),
          filters(_filters),
          info_count(_info_count),
          info_tag(_info_tag),
          linked_record(_linked_record),
          link_name_len(_link_name_len),
          link_name(_link_name),
          reference_box_ID(_reference_box_ID) {}
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
    void write(genie::core::Writer& writer);
    /**
     * @brief
     */
    void write(std::ostream& outputfile) const;

    uint64_t getVariantIndex() const { return variant_index; }
    uint16_t getSeqID() const { return seq_ID; }
    uint64_t getPos() const { return pos; }
    uint8_t getStrand() const { return strand; }
    std::string getID() const { return ID; }
    std::string getDescription() const { return description; }
    std::string getRef() const { return ref; }
    uint8_t getAltCount() const { return alt_count; }

    std::vector<std::string> getAlt() const { return altern; }
    uint32_t getDepth() const { return depth; }
    uint32_t getSeqQual() const { return seq_qual; }
    uint32_t getMapQual() const { return map_qual; }
    uint32_t getMapNumQual0() const { return map_num_qual_0; }
    std::string getFilters() const { return filters; }
    uint8_t getInfoCount() const { return info_count; }
    std::vector<Info_tag> getInfoTag() const { return info_tag; }
    bool isLinkedRecord() const { return (linked_record == 0 ? false : true); }
    std::string getLinkName() const { return link_name; }
    uint8_t getReferenceBoxID() const { return reference_box_ID; }
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
