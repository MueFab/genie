/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

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
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @return
     */
    const genie::format::mgb::AUHeader& getHeader() const;

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
    void write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override;

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
