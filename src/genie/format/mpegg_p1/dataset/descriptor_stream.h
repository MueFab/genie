/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_DESCRIPTOR_STREAM_H
#define GENIE_PART1_DESCRIPTOR_STREAM_H

#include <cstdint>
#include <vector>
#include <list>

#include <genie/util/bitwriter.h>
#include <genie/util/bitreader.h>

#include <access-unit.h>
#include <genie/format/mpegg_p1/dataset/descriptor_stream_header.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class DSProtection {
   private:
    std::vector<uint8_t> DS_protection_value;  //!<

   public:
    /**
     * Default
     */
    DSProtection();
    /**
     *
     * @param bit_reader
     * @param length
     */
    DSProtection(genie::util::BitReader& bit_reader, size_t length);
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

class DescriptorStream {
   private:
    // ISO 23092-1 Section 6.6.4.2
    DescriptorStreamHeader descriptor_stream_header;

    // ISO 23092-1 Section 6.6.4.3
    DSProtection DS_protection;

    // ISO 23092-2
    std::list<uint8_t> block_payload;

   public:
    /**
     * Default
     */
    DescriptorStream();
    /**
     *
     * @param bit_reader
     * @param length
     */
    DescriptorStream(genie::util::BitReader& bit_reader, size_t length);


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

#endif  // GENIE_PART1_DESCRIPTOR_STREAM_H
