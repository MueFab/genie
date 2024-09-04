/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class AccessUnitHeader : public GenInfo {
 private:
    genie::format::mgb::AUHeader header;  //!< @brief
    bool mit_flag;                        //!< @brief

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
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const genie::format::mgb::AUHeader& getHeader() const;

    /**
     * @brief
     * @return
     */
    genie::format::mgb::AUHeader& getHeader();

    /**
     * @brief
     */
    AccessUnitHeader();

    /**
     * @brief
     * @param reader
     * @param parameterSets
     * @param mit
     */
    explicit AccessUnitHeader(util::BitReader& reader,
                              const std::map<size_t, core::parameter::EncodingSet>& parameterSets, bool mit);

    /**
     * @brief
     * @param _header
     * @param _mit_flag
     */
    explicit AccessUnitHeader(genie::format::mgb::AUHeader _header, bool _mit_flag);

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

#endif  // SRC_GENIE_FORMAT_MGG_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
