/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_PACKET_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_PACKET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class PacketHeader {
 private:
    uint16_t sid;             //!< @brief
    uint8_t reserved;         //!< @brief
    bool marker_bit;          //!< @brief
    uint8_t sequence_number;  //!< @brief
    uint16_t packet_size;     //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    uint16_t getSID() const;

    /**
     * @brief
     * @return
     */
    uint8_t getReserved() const;

    /**
     * @brief
     * @return
     */
    bool getMarkerBit() const;

    /**
     * @brief
     * @return
     */
    uint8_t getSequenceNumber() const;

    /**
     * @brief
     * @return
     */
    uint16_t getPacketSize() const;

    /**
     * @brief
     * @param _sid
     * @param _reserved
     * @param _marker_bit
     * @param _sequence_number
     * @param _packet_size
     */
    PacketHeader(uint16_t _sid, uint8_t _reserved, bool _marker_bit, uint8_t _sequence_number, uint16_t _packet_size);

    /**
     * @brief
     * @param reader
     */
    explicit PacketHeader(util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_PACKET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
