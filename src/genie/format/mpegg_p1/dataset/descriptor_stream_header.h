/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_DESCRIPTOR_STREAM_HEADER_H
#define GENIE_PART1_DESCRIPTOR_STREAM_HEADER_H

#include <cstdint>
#include <vector>
#include <genie/util/bitwriter.h>
#include <genie/util/bitreader.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class DescriptorStreamHeader{
   private:

     // u(1)
    uint8_t reserved;

    // u(7)
    uint8_t descriptor_ID;

    // u(4)
    uint8_t class_ID;

    // u(32)
    uint32_t num_blocks;

   public:
    /**
     * Default
     */
    DescriptorStreamHeader();
    /**
     *
     * @param bit_reader
     * @param length
     */
    DescriptorStreamHeader(genie::util::BitReader& bit_reader, size_t length);

    /**
     *
     * @return
     */
    uint64_t getLength() const;
    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;

};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DESCRIPTOR_STREAM_HEADER_H
