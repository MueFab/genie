/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_

#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ReferenceLocation {
 public:
    enum class Flag : bool { INTERNAL = false, EXTERNAL = true };

    ReferenceLocation();

    explicit ReferenceLocation(Flag _flag);

    bool isExternal() const;

    Flag getExternalRefFlag() const;

    virtual uint64_t getLength() const;

    virtual void write(genie::util::BitWriter& writer) const;

 private:
    Flag external_ref_flag;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_REFERENCE_LOCATION_H_
