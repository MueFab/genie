/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/variant_site/record.h"
#include <algorithm>
#include <string>
#include <utility>

#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

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
    for (auto tag : info_tag) {
        outputfile << std::to_string(tag.info_tag_len) << ",";
        outputfile << '"' << tag.info_tag << '"' << ",";
        outputfile << std::to_string(tag.info_type) << ",";
        outputfile << std::to_string(tag.info_array_len) << ",";
        for (auto value : tag.info_value) {
            outputfile << infoToCorrectString(value, tag.info_type);
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
    variant_index = reader.readBypassBE<uint64_t>();
    if (!reader.isGood()) return false;
    seq_ID = reader.readBypassBE<uint16_t>();
    pos = reader.readBypassBE<uint64_t, 5>();
    strand = reader.readBypassBE<uint8_t>();
    ID_len = reader.readBypassBE<uint8_t>();
    if (ID_len > 0) {
        ID.resize(ID_len);
        reader.readBypass(&ID[0], ID_len);
    }
    description_len = reader.readBypassBE<uint8_t>();
    if (description_len > 0) {
        description.resize(description_len);
        reader.readBypass(&description[0], description_len);
    }
    ref_len = reader.readBypassBE<uint32_t>();
    if (ref_len > 0) {
        ref.resize(ref_len);
        reader.readBypass(&ref[0], ref_len);
    }
    alt_count = reader.readBypassBE<uint8_t>();
    for (auto i = 0; i < alt_count; ++i) {
        alt_len.push_back(reader.readBypassBE<uint32_t>());
        std::string altlist(alt_len.back(), 0);
        reader.readBypass(&altlist[0], alt_len.back());
        altern.push_back(altlist);
    }

    depth = reader.readBypassBE<uint32_t, 4>();
    seq_qual = reader.readBypassBE<uint32_t, 4>();
    map_qual = reader.readBypassBE<uint32_t, 4>();
    map_num_qual_0 = reader.readBypassBE<uint32_t, 4>();

    filters_len = reader.readBypassBE<uint8_t>();
    if (filters_len > 0) {
        filters.resize(filters_len);
        reader.readBypass(&filters[0], filters_len);
    }
    info_count = reader.readBypassBE<uint8_t>();
    info_tag.resize(info_count);
    for (auto& infoTag : info_tag) {
        infoTag.info_tag_len = reader.readBypassBE<uint8_t>();
        if (infoTag.info_tag_len > 0) {
            infoTag.info_tag.resize(infoTag.info_tag_len);
            reader.readBypass(&infoTag.info_tag[0], infoTag.info_tag_len);
        }
        infoTag.info_type = reader.readBypassBE<uint8_t>();
        auto inf_size = determineSize(infoTag.info_type);

        infoTag.info_array_len = reader.readBypassBE<uint8_t>();

        if (infoTag.info_array_len > 0) {
            infoTag.info_value.resize(infoTag.info_array_len);
        }

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
        }
    }

    linked_record = reader.readBypassBE<uint8_t, 1>();
    if (linked_record) {
        link_name_len = reader.readBypassBE<uint8_t>();
        if (link_name_len > 0) {
            link_name.resize(link_name_len);
            reader.readBypass(&link_name[0], link_name_len);
        }
        reference_box_ID = reader.readBypassBE<uint8_t>();
    }
    return true;
}

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
