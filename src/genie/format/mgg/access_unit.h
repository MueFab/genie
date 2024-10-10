/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <optional>
#include <string>
#include <vector>
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgg/access_unit_header.h"
#include "genie/format/mgg/au_information.h"
#include "genie/format/mgg/au_protection.h"
#include "genie/format/mgg/block.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class AccessUnit : public GenInfo {
 private:
    AccessUnitHeader header;                      //!< @brief
    std::optional<AUInformation> au_information;  //!< @brief
    std::optional<AUProtection> au_protection;    //!< @brief
    std::vector<Block> blocks;                    //!< @brief

    core::MPEGMinorVersion version;  //!< @brief

 public:
    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

    /**
     * @brief
     * @return
     */
    AUInformation& getInformation();

    /**
     * @brief
     * @return
     */
    AUProtection& getProtection();

    /**
     * @brief
     * @return
     */
    format::mgb::AccessUnit decapsulate();

    /**
     * @brief
     * @param au
     * @param mit
     * @param _version
     */
    AccessUnit(format::mgb::AccessUnit au, bool mit, core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param h
     * @param version
     */
    explicit AccessUnit(AccessUnitHeader h, core::MPEGMinorVersion version);

    /**
     * @brief
     * @param reader
     * @param parameterSets
     * @param mit
     * @param block_header
     * @param version
     */
    AccessUnit(util::BitReader& reader, const std::map<size_t, core::parameter::EncodingSet>& parameterSets, bool mit,
               bool block_header, core::MPEGMinorVersion version);

    /**
     * @brief
     * @param b
     */
    void addBlock(Block b);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<Block>& getBlocks() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const AccessUnitHeader& getHeader() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool hasInformation() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool hasProtection() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const AUInformation& getInformation() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const AUProtection& getProtection() const;

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
    [[nodiscard]] const std::string& getKey() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
