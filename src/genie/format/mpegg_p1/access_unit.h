/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mpegg_p1/access_unit_header.h"
#include "genie/format/mpegg_p1/au_information.h"
#include "genie/format/mpegg_p1/au_protection.h"
#include "genie/format/mpegg_p1/block.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class AccessUnit : public GenInfo {
 private:
    AccessUnitHeader header;                        //!< @brief
    boost::optional<AUInformation> au_information;  //!< @brief
    boost::optional<AUProtection> au_protection;    //!< @brief
    std::vector<Block> blocks;                      //!< @brief

    core::MPEGMinorVersion version;

 public:
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* Access Unit");
        header.print_debug(output, depth + 1, max_depth);
        if (au_information) {
            au_information->print_debug(output, depth + 1, max_depth);
        }
        if (au_protection) {
            au_protection->print_debug(output, depth + 1, max_depth);
        }
        for (const auto& r : blocks) {
            r.print_debug(output, depth + 1, max_depth);
        }
    }

    format::mgb::AccessUnit decapsulate() {
        std::vector<format::mgb::Block> newBlocks;
        for(auto& b : blocks) {
            newBlocks.emplace_back(b.decapsulate());
        }
        return {std::move(header.getHeader()), std::move(newBlocks)};
    }

    AccessUnit(format::mgb::AccessUnit au,  bool mit, core::MPEGMinorVersion _version) : header(std::move(au.getHeader()), mit), version(_version){
        for(auto& b: au.getBlocks()) {
            blocks.emplace_back(std::move(b));
        }
    }

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param h
     */
    explicit AccessUnit(AccessUnitHeader h, core::MPEGMinorVersion);

    /**
     * @brief
     * @param reader
     * @param parameterSets
     * @param mit
     */
    AccessUnit(util::BitReader& reader, const std::map<size_t, core::parameter::EncodingSet>& parameterSets, bool mit,
               bool block_header, core::MPEGMinorVersion);

    /**
     * @brief
     * @param b
     */
    void addBlock(Block b);

    /**
     * @brief
     * @return
     */
    const std::vector<Block>& getBlocks() const;

    /**
     * @brief
     * @return
     */
    const AccessUnitHeader& getHeader() const;

    /**
     * @brief
     * @return
     */
    bool hasInformation() const;

    /**
     * @brief
     * @return
     */
    bool hasProtection() const;

    /**
     * @brief
     * @return
     */
    const AUInformation& getInformation() const;

    /**
     * @brief
     * @return
     */
    const AUProtection& getProtection() const;

    /**
     * @brief
     * @param au
     */
    void setInformation(AUInformation au);

    /**
     * @brief
     * @param au
     */
    void setProtection(AUProtection au);

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
