/**
 * @file packet_header.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `PacketHeader` class for MPEG-G packet header management.
 *
 * The `PacketHeader` class encapsulates the structure of a packet header used within the MPEG-G
 * genomic data format. This class is used for handling packet metadata, including stream identifiers,
 * sequence numbering, and packet size, which are crucial for maintaining the integrity and sequence
 * of data packets in MPEG-G containers.
 *
 * The packet header is a low-level structure used extensively throughout the MPEG-G standard for
 * data encapsulation and transport, making it a key element in managing the flow and order of genomic data.
 */

#ifndef SRC_GENIE_FORMAT_MGG_PACKET_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_PACKET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class PacketHeader
 * @brief Represents the header of a data packet within an MPEG-G container.
 *
 * The `PacketHeader` class provides methods to manage and manipulate the fields of a packet header,
 * including stream identifiers (`SID`), sequence numbers, packet size, and a marker bit to indicate
 * the end of a sequence. This structure is used to manage data flow in an MPEG-G file and ensures
 * that the data is read and written in the correct order.
 */
class PacketHeader {
 private:
    uint16_t sid;             //!< @brief Stream ID for identifying the stream within the MPEG-G file.
    uint8_t reserved;         //!< @brief Reserved field for future use.
    bool marker_bit;          //!< @brief Indicates the end of a sequence of packets.
    uint8_t sequence_number;  //!< @brief Sequence number for the packet, used for ordering.
    uint16_t packet_size;     //!< @brief Size of the packet in bytes.

 public:
    /**
     * @brief Retrieves the Stream ID (`SID`).
     * @return The `SID` of the packet.
     */
    [[nodiscard]] uint16_t getSID() const;

    /**
     * @brief Retrieves the reserved field value.
     * @return The reserved value.
     */
    [[nodiscard]] uint8_t getReserved() const;

    /**
     * @brief Checks if the marker bit is set.
     * @return True if the marker bit is set, false otherwise.
     */
    [[nodiscard]] bool getMarkerBit() const;

    /**
     * @brief Retrieves the sequence number of the packet.
     * @return The sequence number.
     */
    [[nodiscard]] uint8_t getSequenceNumber() const;

    /**
     * @brief Retrieves the size of the packet.
     * @return The packet size in bytes.
     */
    [[nodiscard]] uint16_t getPacketSize() const;

    /**
     * @brief Constructs a `PacketHeader` object with specified values.
     * @param _sid Stream ID.
     * @param _reserved Reserved field.
     * @param _marker_bit Marker bit.
     * @param _sequence_number Sequence number.
     * @param _packet_size Size of the packet in bytes.
     */
    PacketHeader(uint16_t _sid, uint8_t _reserved, bool _marker_bit, uint8_t _sequence_number, uint16_t _packet_size);

    /**
     * @brief Constructs a `PacketHeader` object by reading from a bitstream.
     * @param reader Bitstream reader to extract packet header fields.
     */
    explicit PacketHeader(util::BitReader& reader);

    /**
     * @brief Writes the packet header fields to a bitstream.
     * @param writer Bitstream writer to output the packet header.
     */
    void write(util::BitWriter& writer) const;

    /**
     * @brief Returns the length of the packet header in bits.
     * @return Fixed length of the packet header in bits.
     */
    [[nodiscard]] static uint64_t getLength();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_PACKET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
