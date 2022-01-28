/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_METADATA_H
#define GENIE_DATASET_METADATA_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetMetadata : public GenInfo {
 public:
    const std::string& getKey() const override {
        static const std::string key = "dtmd";
        return key;
    }

    /**
     * @brief
     */
    explicit DatasetMetadata(genie::util::BitReader& bitreader,
                             genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
        : version(_version) {
        auto length = bitreader.readBypassBE<uint64_t>();
        auto metadata_length = length - GenInfo::getSize();
        if (version != genie::core::MPEGMinorVersion::V1900) {
            dataset_group_id = bitreader.readBypassBE<uint8_t>();
            dataset_id = bitreader.readBypassBE<uint16_t>();
            metadata_length -= sizeof(uint8_t);
            metadata_length -= sizeof(uint16_t);
        }
        dg_metatdata_value.resize(metadata_length);
        bitreader.readBypass(dg_metatdata_value);
    }

    DatasetMetadata(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _dg_metatdata_value,
                    genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
        : version(_version),
          dataset_group_id(_dataset_group_id),
          dataset_id(_dataset_id),
          dg_metatdata_value(std::move(_dg_metatdata_value)) {}

    /**
     * @brief
     * @param bitWriter
     */
    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        if (version != genie::core::MPEGMinorVersion::V1900) {
            bitWriter.writeBypassBE(dataset_group_id);
            bitWriter.writeBypassBE(dataset_id);
        }
        bitWriter.writeBypass(dg_metatdata_value.data(), dg_metatdata_value.length());
    }

    uint64_t getSize() const override {
        return GenInfo::getSize() + dg_metatdata_value.size() +
               (version != genie::core::MPEGMinorVersion::V1900 ? sizeof(uint8_t) + sizeof(uint16_t) : 0);
    }

    uint8_t getDatasetGroupID() const { return dataset_group_id; }

    uint16_t getDatasetID() const { return dataset_id; }

    const std::string& getMetadata() const { return dg_metatdata_value; }

 private:
    genie::core::MPEGMinorVersion version;
    uint8_t dataset_group_id;
    uint16_t dataset_id;
    std::string dg_metatdata_value;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATASET_METADATA_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
