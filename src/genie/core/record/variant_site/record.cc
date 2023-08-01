/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "genie/core/arrayType.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

AttributeData::AttributeData()
    : attributeNameLength(0), attributeName(""), attributeType(0), attributeArrayDims(0), attributeID(0) {}

AttributeData::AttributeData(uint8_t length, std::string name, uint16_t attributeID)
    : attributeNameLength(length),
      attributeName(name),
      attributeType(0),
      attributeArrayDims(0),
      attributeID(attributeID) {}

AttributeData::AttributeData(uint8_t length, std::string name, uint8_t type, uint8_t arrayLength, uint16_t attributeID)
    : attributeNameLength(length),
      attributeName(name),
      attributeType(type),
      attributeArrayDims(arrayLength),
      attributeID(attributeID) {}

AttributeData& AttributeData::operator=(const AttributeData& other) {
    attributeNameLength = other.attributeNameLength;
    attributeName = other.attributeName;
    attributeType = other.attributeType;
    attributeArrayDims = other.attributeArrayDims;
    attributeID = other.attributeID;
    return *this;
}

AttributeData::AttributeData(const AttributeData& other) {
    attributeNameLength = other.attributeNameLength;
    attributeName = other.attributeName;
    attributeType = other.attributeType;
    attributeArrayDims = other.attributeArrayDims;
    attributeID = other.attributeID;
}

uint8_t Record::determineSize(uint8_t selectType) const {
    if (selectType == 0) return 0;
    if (selectType < 5) return 1;
    if (selectType < 7) return 2;
    if (selectType < 9 || selectType == 11) return 4;
    return 4;
}

std::string Record::infoToCorrectString(std::string value, uint8_t selectType) const {
    std::string outputstring;
    auto type_size = determineSize(selectType);
    if (selectType == 0) {
        outputstring = '"' + value + '"' + ",";
    } else {
        if (selectType == 0) {
            outputstring = '"' + value + '"' + ",";
        } else if (selectType == 1 || selectType == 2 || selectType == 4 || selectType == 6 || selectType == 8 ||
                   selectType == 10) {
            uint64_t temp = 0;
            memcpy(&temp, value.c_str(), type_size);
            outputstring = std::to_string(temp) + ",";
        } else if (selectType == 3) {
            int8_t temp = 0;
            memcpy(&temp, value.c_str(), type_size);
            outputstring = std::to_string(temp) + ",";

        } else if (selectType == 5) {
            int16_t temp = 0;
            memcpy(&temp, value.c_str(), type_size);
            outputstring = std::to_string(temp) + ",";

        } else if (selectType == 7 || selectType == 11) {
            int32_t temp = 0;
            char bytearray[4];
            for (auto i = 0; i < type_size; ++i) bytearray[i] = value.c_str()[3 - i];
            memcpy(&temp, bytearray, type_size);
            outputstring = std::to_string(temp) + ",";

        } else if (selectType == 9) {
            int64_t temp = 0;
            char bytearray[8];
            for (auto i = 0; i < type_size; ++i) bytearray[i] = value.c_str()[7 - i];
            memcpy(&temp, bytearray, type_size);
            outputstring = std::to_string(temp) + ",";

        } else if (selectType == 11) {
            uint32_t temp = 0;
            memcpy(&temp, value.c_str(), type_size);
            outputstring = std::to_string(temp) + ",";
        }
    }
    return outputstring;
}

Record::Record(util::BitReader& reader) { read(reader); }

void Record::write(core::Writer& writer) {
    writer.write(variant_index, 64);
    writer.write(seq_ID, 16);
    writer.write(pos, 40);
    writer.write(strand, 8);
    writer.write(ID_len, 8);
    writer.write(ID);
    writer.write(description_len, 8);
    writer.write(description);
    writer.write(ref_len, 32);
    writer.write(ref);
    writer.write(alt_count, 8);
    for (auto i = 0; i < alt_count; ++i) {
        writer.write(alt_len[i], 32);
        writer.write(altern[i]);
    }
    writer.write(depth, 32);
    writer.write(seq_qual, 32);
    writer.write(map_qual, 32);
    writer.write(map_num_qual_0, 32);
    writer.write(filters_len, 8);
    writer.write(filters);
    writer.write(info_count, 8);
    for (auto i = 0; i < info_count; ++i) {
        writer.write(info_tag[i].info_tag_len, 8);
        writer.write(info_tag[i].info_tag);
        writer.write(info_tag[i].info_type, 8);
        writer.write(info_tag[i].info_array_len, 8);
        arrayType writeType;
        for (auto j = 0; j < info_tag[i].info_array_len; ++j) {
            writeType.toFile(info_tag[i].info_type, info_tag[i].infoValue[j], writer);
            if (info_tag[i].info_type == 0) writer.write_reserved(8);
        }
    }
    writer.write_reserved(7);
    writer.write(linked_record, 1);
    if (linked_record) {
        writer.write(link_name_len, 8);
        writer.write(link_name);
        writer.write(reference_box_ID, 8);
    }
}

void Record::write(std::ostream& outputfile) const {
    outputfile << std::to_string(variant_index) << ",";
    outputfile << std::to_string(seq_ID) << ",";
    outputfile << std::to_string(pos) << ",";
    outputfile << std::to_string(strand) << ",";
    outputfile << std::to_string(ID_len) << ",";
    outputfile << '"' << ID << '"' << ",";
    outputfile << std::to_string(description_len) << ",";
    outputfile << '"' << description << '"' << ",";
    outputfile << std::to_string(ref_len) << ",";
    outputfile << '"' << ref << '"' << ",";
    outputfile << std::to_string(alt_count) << ",";

    for (auto i = 0; i < alt_count; ++i) {
        outputfile << std::to_string(alt_len[i]) << ",";
        outputfile << '"' << altern[i] << '"' << ",";
    }

    outputfile << std::to_string(depth) << ",";

    outputfile << std::to_string(seq_qual) << ",";
    outputfile << std::to_string(map_qual) << ",";
    outputfile << std::to_string(map_num_qual_0) << ",";
    outputfile << std::to_string(filters_len) << ",";
    outputfile << '"' << filters << '"' << ",";
    outputfile << std::to_string(info_count) << ",";
    for (const auto& tag : info_tag) {
        outputfile << std::to_string(tag.info_tag_len) << ",";
        outputfile << '"' << tag.info_tag << '"' << ",";
        outputfile << std::to_string(tag.info_type) << ",";
        outputfile << std::to_string(tag.info_array_len) << ",";
        for (auto value : tag.infoValue) {
        }
    }

    outputfile << std::to_string(linked_record) << ",";
    if (linked_record == 1) {
        outputfile << std::to_string(link_name_len) << ",";
        outputfile << '"' << link_name << '"' << ",";
        outputfile << std::to_string(reference_box_ID);
    }
}

bool Record::read(genie::util::BitReader& reader) {
    variant_index = reader.read_b(64);
    if (!reader.isGood()) return false;
    seq_ID = static_cast<uint16_t>(reader.read_b(16));
    pos = reader.read_b(40);
    strand = static_cast<uint8_t>(reader.read_b(8));
    ID_len = static_cast<uint8_t>(reader.read_b(8));
    if (ID_len > 0) {
        ID.resize(ID_len);
        reader.readBypass(&ID[0], ID_len);
    }
    description_len = static_cast<uint8_t>(reader.read_b(8));
    ;
    if (description_len > 0) {
        description.resize(description_len);
        reader.readBypass(&description[0], description_len);
    }
    ref_len = reader.readBypassBE<uint32_t>();
    if (ref_len > 0) {
        ref.resize(ref_len);
        reader.readBypass(&ref[0], ref_len);
    }
    alt_count = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < alt_count; ++i) {
        alt_len.push_back(static_cast<uint32_t>(reader.read_b(32)));
        std::string altlist(alt_len.back(), 0);
        reader.readBypass(&altlist[0], alt_len.back());
        altern.push_back(altlist);
    }

    depth = static_cast<uint32_t>(reader.read_b(32));

    seq_qual = static_cast<uint32_t>(reader.read_b(32));
    map_qual = static_cast<uint32_t>(reader.read_b(32));
    map_num_qual_0 = static_cast<uint32_t>(reader.read_b(32));

    filters_len = static_cast<uint8_t>(reader.read_b(8));
    if (filters_len > 0) {
        filters.resize(filters_len);
        reader.readBypass(&filters[0], filters_len);
    }
    info_count = static_cast<uint8_t>(reader.read_b(8));
    info_tag.resize(info_count);
    for (auto& infoTag : info_tag) {
        infoTag.info_tag_len = static_cast<uint8_t>(reader.read_b(8));
        if (infoTag.info_tag_len > 0) {
            infoTag.info_tag.resize(infoTag.info_tag_len);
            reader.readBypass(&infoTag.info_tag[0], infoTag.info_tag_len);
        }
        infoTag.info_type = static_cast<uint8_t>(reader.read_b(8));
        //  auto inf_size = determineSize(infoTag.info_type);

        infoTag.info_array_len = static_cast<uint8_t>(reader.read_b(8));

        if (infoTag.info_array_len > 0) {
            infoTag.infoValue.resize(infoTag.info_array_len);
            arrayType infoArray;
            for (auto& value : infoTag.infoValue) {
                value = infoArray.toArray(infoTag.info_type, reader);
            }
        }
        /*
        for (auto& inf : infoTag.info_value) {
            if (inf_size == 0) {
                uint8_t read = 0xF;
                do {
                    read = reader.readBypassBE<uint8_t>();
                    if (read != 0) inf += read;
                } while (read != 0);
            } else {
                inf.resize(inf_size);
                reader.readBypass(&inf[0], inf_size);
            }
        }*/
    }

    linked_record = static_cast<uint8_t>(reader.read_b(8));
    if (linked_record) {
        link_name_len = static_cast<uint8_t>(reader.read_b(8));
        if (link_name_len > 0) {
            link_name.resize(link_name_len);
            reader.readBypass(&link_name[0], link_name_len);
        }
        reference_box_ID = static_cast<uint8_t>(reader.read_b(8));
    }
    return true;
}

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
