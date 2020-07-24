#ifndef GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_H
#define GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_H

#include <cstdint>

#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class Checksum {
   public:
    enum class Algo: uint8_t{
        MD5 = 0,
        SHA256 = 1
    };

    explicit Checksum(Algo _algo);

    Algo getType();

    virtual void write(genie::util::BitWriter& bit_writer) const;

   protected:
    Algo checksum_alg;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_H
