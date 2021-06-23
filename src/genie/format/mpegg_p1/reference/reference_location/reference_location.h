/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class ReferenceLocation {
 public:
    /**
     * @brief
     */
    enum class Flag : bool { INTERNAL = false, EXTERNAL = true };

    /**
     * @brief
     */
    ReferenceLocation();

    /**
     * @brief
     */
    virtual ~ReferenceLocation() = default;

    /**
     * @brief
     * @param _flag
     */
    explicit ReferenceLocation(Flag _flag);

    /**
     * @brief
     * @return
     */
    bool isExternal() const;

    /**
     * @brief
     * @return
     */
    Flag getExternalRefFlag() const;

    /**
     * @brief
     * @return
     */
    virtual uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    virtual void write(genie::util::BitWriter& writer) const;

 private:
    Flag external_ref_flag;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
