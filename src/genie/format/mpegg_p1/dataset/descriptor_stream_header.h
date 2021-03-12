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
     * @param _res
     * @param _descriptor_ID
     * @param _class_ID
     * @param _num_blocks
     */
    explicit DescriptorStreamHeader(uint8_t _res, uint8_t _descriptor_ID,
                                    uint8_t _class_ID, uint32_t _num_blocks);
    /**
     *
     * @param bit_reader
     * @param length
     */
    explicit DescriptorStreamHeader(util::BitReader& bit_reader);

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
