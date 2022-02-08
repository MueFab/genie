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
