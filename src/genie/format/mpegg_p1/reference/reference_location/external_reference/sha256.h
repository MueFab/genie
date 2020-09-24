#ifndef GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_SHA256_H
#define GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_SHA256_H

#include <array>
#include <vector>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exception.h>
#include <genie/util/runtime-exception.h>

#include "checksum.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class Sha256: public Checksum {
   private:
    std::array<uint64_t, 4> data;

   public:
    Sha256();

    explicit Sha256(util::BitReader& reader);

    explicit Sha256(std::vector<uint64_t> &_data);

    void write(genie::util::BitWriter& bit_writer) const override;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_SHA256_H
