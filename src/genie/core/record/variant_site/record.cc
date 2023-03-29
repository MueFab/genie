/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "record.h"
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

Record::Record(util::BitReader& reader) {
    variant_index = (reader.readBypassBE<uint64_t>());
    seq_ID = (reader.readBypassBE<uint16_t>());
    pos = (reader.readBypassBE<uint64_t, 5>());
    strand = (reader.readBypassBE<uint8_t>());
    ID_len = (reader.readBypassBE<uint8_t>());
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
        Alt alt_inst;
        alt_inst.alt_len = reader.readBypassBE<uint32_t>();
        if (alt_inst.alt_len > 0) {
            alt_inst.alt.resize(alt_inst.alt_len);
            reader.readBypass(&alt_inst.alt[0], alt_inst.alt_len);
        }
        alt.push_back(alt_inst);
    }

    depth = reader.readBypassBE<float, 4>();
    seq_qual = reader.readBypassBE<float, 4>();
    map_qual = reader.readBypassBE<float, 4>();
    map_num_qual_0 = reader.readBypassBE<float, 4>();

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
            //   std::vector<uint64_t> info_value(infoTag.info_array_len);
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

    for (auto alt_inst : alt) {
        outputfile << std::to_string(alt_inst.alt_len) << ",";
        outputfile << '"' << alt_inst.alt << '"' << ",";
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
        uint8_t type_size = determineSize(tag.info_type);
        for (auto value : tag.info_value) {
            if (tag.info_type == 0)
                outputfile << '"' << value << '"' << ",";
            else {
                if (tag.info_type == 0) {
                    outputfile << '"' << value << '"' << ",";
                } else if (tag.info_type == 1 || tag.info_type == 2 || tag.info_type == 4 || tag.info_type == 6 ||
                           tag.info_type == 8 || tag.info_type == 10) {
                    uint64_t temp = 0;
                    memcpy(&temp, value.c_str(), type_size);
                    /* auto shift = 8 * type_size - 8;
                    for (auto value_byte : value) {
                        temp += static_cast<uint64_t>(value_byte) << shift;
                        shift -= 8;
                    }*/
                    outputfile << std::to_string(temp) << ",";
                } else if (tag.info_type == 3) {
                    int8_t temp = 0;
                    memcpy(&temp, value.c_str(), type_size);
                    outputfile << std::to_string(temp) << ",";

                } else if (tag.info_type == 5) {
                    int16_t temp = 0;
                    memcpy(&temp, value.c_str(), type_size);
                    outputfile << std::to_string(temp) << ",";

                } else if (tag.info_type == 7 || tag.info_type == 11) {
                    int32_t temp = 0;
                    char bytearray[4];
                    for (auto i = 0; i < type_size; ++i) bytearray[i] = value.c_str()[3 - i];
                    memcpy(&temp, bytearray, type_size);
                    outputfile << std::to_string(temp) << ",";
                    
                } else if (tag.info_type == 9) {
                    int64_t temp = 0;
                    char bytearray[8];
                    for (auto i = 0; i < type_size; ++i) bytearray[i] = value.c_str()[7 - i];
                    memcpy(&temp, bytearray, type_size);
                     outputfile << std::to_string(temp) << ",";
 
                } else if (tag.info_type == 11) {
                    int32_t temp = 0;
                    memcpy(&temp, value.c_str(), type_size);
                    outputfile << std::to_string(temp) << ",";
                }
            }
        }
    }

    outputfile << std::to_string(linked_record) << ",";
    if (linked_record == 1) {
        outputfile << std::to_string(link_name_len) << ",";
        outputfile << '"' << link_name << '"' << ",";
        outputfile << std::to_string(reference_box_ID);
    }
}

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
