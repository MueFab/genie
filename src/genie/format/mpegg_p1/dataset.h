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
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "signature_cfg.h"
#include "genie/format/mpegg_p1/access_unit_header.h"
#include "genie/format/mpegg_p1/au_information.h"
#include "genie/format/mpegg_p1/au_protection.h"
#include "genie/format/mpegg_p1/block.h"
#include "genie/format/mpegg_p1/dataset_metadata.h"
#include "genie/format/mpegg_p1/dataset_protection.h"
#include "genie/format/mpegg_p1/dataset_parameter_set.h"
#include "genie/format/mpegg_p1/master_index_table.h"
#include "genie/format/mpegg_p1/access_unit.h"
#include "genie/format/mpegg_p1/descriptor_stream.h"

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
    boost::optional<DatasetMetadata> protection;
    std::vector<DatasetParameterSet> parameterSets;
    boost::optional<MasterIndexTable> master_index_table;
    std::vector<AccessUnit> access_units;
    std::vector<DescriptorStream> descriptor_streams;
 public:


    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
    }

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
