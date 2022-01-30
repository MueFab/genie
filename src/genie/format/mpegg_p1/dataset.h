/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_

#include <memory>
#include <string>
#include <vector>
#include "dataset_header.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class-type.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mpegg_p1/access_unit.h"
#include "genie/format/mpegg_p1/access_unit_header.h"
#include "genie/format/mpegg_p1/au_information.h"
#include "genie/format/mpegg_p1/au_protection.h"
#include "genie/format/mpegg_p1/block.h"
#include "genie/format/mpegg_p1/dataset_metadata.h"
#include "genie/format/mpegg_p1/dataset_parameter_set.h"
#include "genie/format/mpegg_p1/dataset_protection.h"
#include "genie/format/mpegg_p1/descriptor_stream.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/master_index_table.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "signature_cfg.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Dataset : public GenInfo {
 private:
    DatasetHeader header;
    boost::optional<DatasetMetadata> metadata;
    boost::optional<DatasetProtection> protection;
    std::vector<DatasetParameterSet> parameterSets;
    boost::optional<MasterIndexTable> master_index_table;
    std::vector<AccessUnit> access_units;
    std::vector<DescriptorStream> descriptor_streams;
    core::MPEGMinorVersion version;

    std::map<size_t, core::parameter::EncodingSet> encoding_sets;

 public:
    Dataset(util::BitReader& reader, core::MPEGMinorVersion _version) : version(_version) {
        auto start_pos = reader.getPos();
        auto length = reader.readBypassBE<uint64_t>();
        auto end_pos = start_pos + static_cast<int64_t>(length) - 4;
        header = DatasetHeader(reader);
        while (reader.getPos() < end_pos) {
            auto tmp_pos = reader.getPos();
            std::string tmp_str(4, '\0');
            reader.readBypass(tmp_str);
            if (tmp_str == "dtmd") {
                UTILS_DIE_IF(metadata != boost::none, "Metadata already present");
                UTILS_DIE_IF(protection != boost::none, "Metadata must be before protection");
                UTILS_DIE_IF(!parameterSets.empty(), "Metadata must be before parametersets");
                UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
                UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
                UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
                metadata = DatasetMetadata(reader);
            } else if (tmp_str == "dtpr") {
                UTILS_DIE_IF(protection != boost::none, "Protection already present");
                UTILS_DIE_IF(!parameterSets.empty(), "Metadata must be before parametersets");
                UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
                UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
                UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
                protection = DatasetProtection(reader);
            } else if (tmp_str == "pars") {
                UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
                UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
                UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
                parameterSets.emplace_back(reader, version, header.getParameterUpdateFlag());
                encoding_sets.emplace(std::make_pair(size_t(parameterSets.back().getParameterSetID()),
                                                     parameterSets.back().getEncodingSet()));
            } else if (tmp_str == "mitb") {
                UTILS_DIE_IF(master_index_table != boost::none, "MIT already present");
                UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
                UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
                master_index_table = MasterIndexTable(reader, header);
            } else if (tmp_str == "aucn") {
                UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
                access_units.emplace_back(reader, encoding_sets, header.isMITEnabled());
            } else if (tmp_str == "dscn") {
                UTILS_DIE_IF(master_index_table == boost::none, "descriptor streams without MIT not allowed");
                UTILS_DIE_IF(header.isBlockHeaderEnabled(), "descriptor streams only allowed without block headers");
                // TODO: How to calculate offsets
           //     descriptor_streams.emplace_back(reader, master_index_table.get);
            } else {
                UTILS_DIE("Unknown box");
            }
        }
    }

    const DatasetHeader& getHeader() const { return header; }

    void write(genie::util::BitWriter& bitWriter) const override { GenInfo::write(bitWriter); }

    uint64_t getSize() const override {
        std::stringstream stream;
        genie::util::BitWriter writer(&stream);
        write(writer);
        return stream.str().length();
    }

    const std::string& getKey() const override {
        static const std::string key = "dtcn";
        return key;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
