/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_VERSION_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_VERSION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference {

/**
 * @brief
 */
class Version {
 private:
    uint16_t v_major;  //!< @brief
    uint16_t v_minor;  //!< @brief
    uint16_t v_patch;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const Version& other) const;

    /**
     * @brief
     * @param _v_major
     * @param _v_minor
     * @param _v_patch
     */
    Version(uint16_t _v_major, uint16_t _v_minor, uint16_t _v_patch);

    /**
     * @brief
     * @param reader
     */
    explicit Version(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getMajor() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getMinor() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getPatch() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_VERSION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
