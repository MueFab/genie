/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_GEN_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_GEN_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mpegg_p1/box.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class GenInfo : public Box {
 protected:
    virtual void box_write(genie::util::BitWriter& bitWriter) const = 0;
 public:

    static uint64_t getHeaderLength() {
        return sizeof(uint64_t) + sizeof(char) * 4;
    }

    /**
     * @brief
     * @return
     */
    virtual const std::string& getKey() const = 0;

    /**
     * @brief
     * @param bitWriter
     */
    void write(genie::util::BitWriter& bitWriter) const final {
        int64_t begin = bitWriter.getPosition();
        bitWriter.writeBypass(getKey().data(), getKey().length());
        int64_t size_pos = bitWriter.getPosition();
        bitWriter.writeBypassBE<uint64_t>(0);
        box_write(bitWriter);
        int64_t end = bitWriter.getPosition();
        bitWriter.setPosition(size_pos);
        bitWriter.writeBypassBE<uint64_t>(end - begin);
        bitWriter.setPosition(end);
    }

    /**
     * @brief
     * @param info
     * @return
     */
    virtual bool operator==(const GenInfo& info) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_GEN_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
