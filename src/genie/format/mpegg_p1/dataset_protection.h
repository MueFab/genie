/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_PROTECTION_H
#define GENIE_DATASET_PROTECTION_H

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
class DatasetProtection : public GenInfo {
 public:
    const std::string& getKey() const override {
        static const std::string key = "dtpr";
        return key;
    }

    /**
     * @brief
     */
    explicit DatasetProtection(genie::util::BitReader& bitreader,
                                    genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
        : version(_version) {
        auto length = bitreader.readBypassBE<uint64_t>();
        auto protection_length = length - GenInfo::getSize();
        if (version != genie::core::MPEGMinorVersion::V1900) {
            dataset_group_id = bitreader.readBypassBE<uint8_t>();
            dataset_id = bitreader.readBypassBE<uint16_t>();
            protection_length -= sizeof(uint8_t);
            protection_length -= sizeof(uint16_t);
        }
        dg_protection_value.resize(protection_length);
        bitreader.readBypass(dg_protection_value);
    }

    DatasetProtection(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _dg_protection_value,
                           genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
        : version(_version),
          dataset_group_id(_dataset_group_id),
          dataset_id(_dataset_id),
          dg_protection_value(std::move(_dg_protection_value)) {}

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
        bitWriter.writeBypass(dg_protection_value.data(), dg_protection_value.length());
    }

    uint64_t getSize() const override {
        return GenInfo::getSize() + dg_protection_value.size() +
               (version != genie::core::MPEGMinorVersion::V1900 ? sizeof(uint8_t) + sizeof(uint16_t) : 0);
    }

    uint8_t getDatasetGroupID() const { return dataset_group_id; }

    uint16_t getDatasetID() const { return dataset_id; }

    const std::string& getProtection() const { return dg_protection_value; }

 private:
    genie::core::MPEGMinorVersion version;
    uint8_t dataset_group_id;
    uint16_t dataset_id;
    std::string dg_protection_value;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATASET_PROTECTION_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
