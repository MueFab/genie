#ifndef GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_SHA256_H
#define GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_SHA256_H

#include <array>
#include <vector>

#include "checksum.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class Sha256: Checksum {
   private:
    std::array<uint64_t, 4> data;

   public:
    Sha256();

    explicit Sha256(std::vector<uint64_t> &_data);

    void write(genie::util::BitWriter& bit_writer) const override;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_SHA256_H
