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
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        static const std::string class_lut[] = {"NONE", "P", "N", "M", "I", "HM", "U"};
        print_offset(output, depth, max_depth, "* Access Unit Header");
        print_offset(output, depth + 1, max_depth, "Access unit ID: " + std::to_string(int(header.getID())));
        print_offset(output, depth + 1, max_depth, "Access unit class: " + class_lut[(int)header.getClass()]);
        print_offset(output, depth + 1, max_depth, "Access unit blocks: " + std::to_string((int)header.getNumBlocks()));
        print_offset(output, depth + 1, max_depth,
                     "Access unit records: " + std::to_string((int)header.getReadCount()));
        print_offset(output, depth + 1, max_depth,
                     "Access unit parameter set ID: " + std::to_string((int)header.getParameterID()));
        if (header.getClass() != core::record::ClassType::CLASS_U) {
            print_offset(output, depth + 1, max_depth,
                         "Access unit reference ID: " + std::to_string((int)header.getAlignmentInfo().getRefID()));
            print_offset(output, depth + 1, max_depth,
                         "Access unit start pos:  " + std::to_string((int)header.getAlignmentInfo().getStartPos()));
            print_offset(output, depth + 1, max_depth,
                         "Access unit end pos:  " + std::to_string((int)header.getAlignmentInfo().getEndPos()));
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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
