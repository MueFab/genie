/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class PacketHeader {
 private:
    uint16_t sid;
    uint8_t reserved;
    bool marker_bit;
    uint8_t sequence_number;
    uint16_t packet_size;

 public:

    uint16_t getSID() const {
        return sid;
    }

    uint8_t getReserved() const {
        return reserved;
    }

    bool getMarkerBit() const {
        return marker_bit;
    }

    uint8_t getSequenceNumber() const {
        return sequence_number;
    }

    uint16_t getPacketSize() const {
        return packet_size;
    }

    PacketHeader(uint16_t _sid, uint8_t _reserved, bool _marker_bit, uint8_t _sequence_number, uint16_t _packet_size) :
     sid(_sid), reserved(_reserved), marker_bit(_marker_bit), sequence_number(_sequence_number), packet_size(_packet_size) {
    }
    explicit PacketHeader(util::BitReader& reader) {
        sid = reader.read<uint16_t>(13);
        reserved = reader.read<uint8_t>(3);
        marker_bit = reader.read<bool>(1);
        sequence_number = reader.read<uint8_t>(8);
        packet_size = reader.read<uint16_t>(15);
    }

    void write(util::BitWriter& writer) const {
        writer.write(sid, 13);
        writer.write(reserved, 3);
        writer.write(marker_bit, 1);
        writer.write(sequence_number, 8);
        writer.write(packet_size, 15);
    }

    uint64_t getLength() const {
        return 5;
    }

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
