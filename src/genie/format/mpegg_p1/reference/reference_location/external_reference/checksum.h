#ifndef GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_H
#define GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_H

#include <cstdint>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class Checksum {
   public:
    enum class Algo: uint8_t{
        MD5 = 0,
        SHA256 = 1
    };

   private:
    Algo checksum_alg;

   public:

    Checksum();

    explicit Checksum(Algo _algo);

    Algo getType() const;

    virtual uint64_t getLength() const;

    virtual void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_H
