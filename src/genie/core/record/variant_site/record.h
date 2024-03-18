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

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

class InfoFields {
 public:
    using CustomType = std::vector<uint8_t>;
    struct Field {
        std::string tag;
        genie::core::DataType type;
        std::vector<CustomType> values;
    };
    void read(genie::util::BitReader& reader) {
        auto alt_count = static_cast<uint8_t>(reader.read_b(8));
        fields.reserve(alt_count * 16);
        for (auto i = 0; i < alt_count; ++i) {
            auto alt_len = static_cast<uint8_t>(reader.read_b(8));
            std::string alttag(alt_len, 0);
            for (auto& tag : alttag) tag = reader.readBypassBE<uint8_t>();
            auto alt_type = static_cast<genie::core::DataType>(reader.readBypassBE<uint8_t>());
            auto arrayLength = reader.readBypassBE<uint8_t>();
            ArrayType arrayType;
            std::vector<CustomType> values;
            for (auto j = 0; j < arrayLength; ++j) {
                std::vector<uint8_t> value = arrayType.toArray(alt_type, reader);
                values.push_back(value);
            }
            
            Field field{alttag, alt_type, values};
            fields.push_back(field);
        }
    }
    void clear() {
        fields.clear();
        fields.shrink_to_fit();
    }
    std::vector<Field>& getFields() { return fields; }

 private:
    std::vector<Field> fields;
};

struct Info_tag {
    uint8_t info_tag_len;
    std::string info_tag;
    genie::core::DataType info_type;
    uint8_t info_array_len;
    std::vector<std::vector<uint8_t>> infoValue;
};

struct AlternativeAllele {
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

    std::vector<uint32_t> alt_len;
    std::vector<std::string> altern;

    uint32_t depth;
    uint32_t seq_qual;
    uint32_t map_qual;
    uint32_t map_num_qual_0;
    uint8_t filters_len;
    std::string filters;

    InfoFields info;

    uint8_t linked_record;
    uint8_t link_name_len;
    std::string link_name;
    uint8_t reference_box_ID;

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
          linked_record(0),
          link_name_len(0),
          link_name(""),
          reference_box_ID(0) {}
    /**
     * @brief
     */
    Record(uint64_t _variant_index, uint16_t _seq_ID, uint64_t _pos, uint8_t _strand, uint8_t _ID_len, std::string& _ID,
           uint8_t _description_len, std::string _description, uint32_t _ref_len, std::string _ref, uint8_t _alt_count,
           std::vector<uint32_t> _alt_len, std::vector<std::string> _altern, uint32_t _depth, uint32_t _seq_qual,
           uint32_t _map_qual, uint32_t _map_num_qual_0, uint8_t _filters_len, std::string _filters,
            uint8_t _linked_record, uint8_t _link_name_len,
           std::string _link_name, uint8_t _reference_box_ID)
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
  //  Record(genie::util::BitReader& reader, std::vector<Info_tag> infoTag) : info_tag(infoTag) { read(reader); }
 //   Record(genie::util::BitReader& reader) { read(reader); }
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
    std::vector<InfoFields::Field>& getInfoTag() { return info.getFields(); }
  //  std::vector<std::vector<std::vector<uint8_t>>> getInfoValues() const { return infoValue; }
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
