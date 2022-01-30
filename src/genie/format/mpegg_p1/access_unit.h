/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_

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

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class AccessUnit : public GenInfo {
 private:
    AccessUnitHeader header;
    boost::optional<AUInformation> au_information;
    boost::optional<AUProtection> au_protection;
    std::vector<Block> blocks;
 public:

    AccessUnit(AccessUnitHeader h) : header(std::move(h)){

    }

    AccessUnit(util::BitReader& reader, const std::map<size_t, core::parameter::ParameterSet> &parameterSets, bool mit) {
        reader.readBypassBE<uint64_t>();
        header = AccessUnitHeader(reader, parameterSets, mit);
        do {
            auto tmp_pos = reader.getPos();
            std::string tmp_str(4, '\0');
            reader.readBypass(tmp_str);
            if(tmp_str == "auin") {
                UTILS_DIE_IF(au_information != boost::none, "AU-Inf already present");
                UTILS_DIE_IF(au_protection != boost::none, "AU-Inf must be before AU-PR");
                au_information = AUInformation(reader);
            } else if (tmp_str == "aupr") {
                UTILS_DIE_IF(au_protection != boost::none, "AU-Pr already present");
                au_protection = AUProtection(reader);
            } else {
                reader.setPos(tmp_pos);
                break;
            }
        } while(true);
        for (size_t i = 0; i < header.getHeader().getNumBlocks(); ++i) {
            blocks.emplace_back(reader);
        }
    }

    void addBlock(Block b) {
        blocks.emplace_back(std::move(b));
    }

    const std::vector<Block>& getBlocks() const {
        return blocks;
    }

    const AccessUnitHeader& getHeader() const {
        return header;
    }

    bool hasInformation() const {
        return au_information != boost::none;
    }

    bool hasProtection() const {
        return au_protection != boost::none;
    }

    const AUInformation& getInformation() const {
        return *au_information;
    }

    const AUProtection& getProtection() const {
        return *au_protection;
    }

    void setInformation (AUInformation au) {
        au_information = std::move(au);
    }

    void setProtection (AUProtection au) {
        au_protection = std::move(au);
    }

    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        header.write(bitWriter);
        if(au_information != boost::none) {
            au_information->write(bitWriter);
        }
        if(au_protection != boost::none) {
            au_protection->write(bitWriter);
        }
        for(const auto& b : blocks) {
            b.write(bitWriter);
        }
    }

    uint64_t getSize() const override {
        std::stringstream stream;
        genie::util::BitWriter writer(&stream);
        write(writer);
        return stream.str().length();
    }

    const std::string& getKey() const override {
        static const std::string key = "aucn";
        return key;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
