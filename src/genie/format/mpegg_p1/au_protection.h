/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_AU_PROTECTION_H
#define GENIE_AU_PROTECTION_H

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
class AUProtection : public GenInfo {
 public:
    const std::string& getKey() const override {
        static const std::string key = "aupr";
        return key;
    }

    /**
     * @brief
     */
    explicit AUProtection(genie::util::BitReader& bitreader,
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
        au_protection_value.resize(metadata_length);
        bitreader.readBypass(au_protection_value);
    }

    AUProtection(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _au_protection_value,
                    genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
        : version(_version),
          dataset_group_id(_dataset_group_id),
          dataset_id(_dataset_id),
          au_protection_value(std::move(_au_protection_value)) {}

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
        bitWriter.writeBypass(au_protection_value.data(), au_protection_value.length());
    }

    uint64_t getSize() const override {
        return GenInfo::getSize() + au_protection_value.size() +
               (version != genie::core::MPEGMinorVersion::V1900 ? sizeof(uint8_t) + sizeof(uint16_t) : 0);
    }

    uint8_t getDatasetGroupID() const { return dataset_group_id; }

    uint16_t getDatasetID() const { return dataset_id; }

    const std::string& getInformation() const { return au_protection_value; }

 private:
    genie::core::MPEGMinorVersion version;
    uint8_t dataset_group_id;
    uint16_t dataset_id;
    std::string au_protection_value;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_AU_PROTECTION_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
