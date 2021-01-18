#ifndef GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_MD5_H
#define GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_MD5_H

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

class Md5: public Checksum {
   private:
    std::array<uint64_t, 2> data;

   public:

    /**
     * Default constructor
     *
     */
    Md5();

    /**
     * Read MD5 checksum from reader
     *
     * @param reader
     */
    explicit Md5(util::BitReader& reader);

    /**
     * Initialize MD5 from array
     *
     * @param _data
     */
    explicit Md5(std::vector<uint64_t> &_data);

    uint64_t getLength() const override;

    /**
     *
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const override;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_CHECKSUM_MD5_H
