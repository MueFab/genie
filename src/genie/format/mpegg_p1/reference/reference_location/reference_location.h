#ifndef GENIE_PART1_REFERENCE_LOCATION_H
#define GENIE_PART1_REFERENCE_LOCATION_H

#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ReferenceLocation{
   public:
    enum class Flag: bool {
        INTERNAL = false,
        EXTERNAL = true
    };

    ReferenceLocation();

    virtual ~ReferenceLocation() = default;

    explicit ReferenceLocation(Flag _flag);

    bool isExternal() const;

    ReferenceLocation::Flag getExternalRefFlag() const;

    virtual uint64_t getLength() const {return 0;}

    virtual void write(genie::util::BitWriter&) const {}

   private:
    Flag external_ref_flag;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_REFERENCE_LOCATION_H
