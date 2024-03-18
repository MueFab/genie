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

#include "genie/core/arrayType.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {


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

    auto info_tag = info.getFields();
    writer.write(static_cast<uint8_t>(info_tag.size()), 8);
    for (auto i = 0; i < info_tag.size(); ++i) {
        writer.write(info_tag[i].tag.size(), 8);
        writer.write(info_tag[i].tag);
        writer.write(static_cast<uint8_t>(info_tag[i].type), 8);
        writer.write(info_tag[i].values.size(), 8);
        ArrayType writeType;
        for (auto j = 0; j < info_tag[i].values.size(); ++j) {
            writeType.toFile(info_tag[i].type, info_tag.at(i).values.at(j), writer);
            if (info_tag[i].type == DataType::STRING) writer.write_reserved(8);
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

bool Record::read(genie::util::BitReader& reader) {
    clearData();
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

    if (description_len > 0) {
        description.resize(description_len);
        reader.readBypass(&description[0], description_len);
    }
    ref_len = static_cast<uint32_t>(reader.read_b(32));
    if (ref_len > 0) {
        ref.resize(ref_len);
        reader.readBypass(&ref[0], ref_len);
    }
    alt_count = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < alt_count; ++i) {
        alt_len.push_back(static_cast<uint32_t>(reader.read_b(32)));
        std::string altlist(alt_len.back(), 0);
        for (auto& item : altlist) item = static_cast<char>(reader.read_b(8));
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

    info.read(reader);
 
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

void Record::clearData() {
    variant_index = 0;
    seq_ID = 0;
    pos = 0;
    strand = 0;
    ID_len = 0;
    ID = "";
    description_len = 0;
    description = "";
    ref_len = 0;
    ref = "";
    alt_count = 0;
    alt_len = {};
    altern = {};
    depth = 0;
    seq_qual = 0;
    map_qual = 0;
    map_num_qual_0 = 0;
    filters_len = 0;
    filters = "";
 
    info.clear();
    
    linked_record = 0;
    link_name_len = 0;
    link_name = "";
    reference_box_ID = 0;
}

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
