/**
 * Copyright 2018-2024 The Genie Authors.
 * @file packet_header.h
 * @copyright This file is part of Genie.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `PacketHeader` class for MPEG-G packet header management.
 *
 * The `PacketHeader` class encapsulates the structure of a packet header used
 * within the MPEG-G genomic data format. This class is used for handling packet
 * metadata, including stream identifiers, sequence numbering, and packet Size,
 * which are crucial for maintaining the integrity and sequence of data packets
 * in MPEG-G containers.
 *
 * The packet header is a low-level structure used extensively throughout the
 * MPEG-G standard for data encapsulation and transport, making it a key element
 * in managing the flow and order of genomic data.
 */

#ifndef SRC_GENIE_FORMAT_MGG_PACKET_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_PACKET_HEADER_H_

// -----------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class PacketHeader
 * @brief Represents the header of a data packet within an MPEG-G container.
 *
 * The `PacketHeader` class provides methods to manage and manipulate the fields
 * of a packet header, including stream identifiers (`SID`), sequence numbers,
 * packet Size, and a marker bit to indicate the end of a sequence. This
 * structure is used to manage data flow in an MPEG-G file and ensures that the
 * data is read and written in the correct order.
 */
class PacketHeader {
  uint16_t sid_;  //!< @brief Stream ID for identifying the stream within the
                  //!< MPEG-G file.
  uint8_t reserved_;  //!< @brief Reserved field for future use.
  bool marker_bit_;   //!< @brief Indicates the end of a sequence of packets.
  uint8_t sequence_number_;  //!< @brief Sequence number for the packet, used
                             //!< for ordering.
  uint16_t packet_size_;     //!< @brief Size of the packet in bytes.

 public:
  /**
   * @brief Retrieves the Stream ID (`SID`).
   * @return The `SID` of the packet.
   */
  [[nodiscard]] uint16_t GetSid() const;

  /**
   * @brief Retrieves the reserved field value.
   * @return The reserved value.
   */
  [[nodiscard]] uint8_t GetReserved() const;

  /**
   * @brief Checks if the marker bit is set.
   * @return True if the marker bit is set, false otherwise.
   */
  [[nodiscard]] bool GetMarkerBit() const;

  /**
   * @brief Retrieves the sequence number of the packet.
   * @return The sequence number.
   */
  [[nodiscard]] uint8_t GetSequenceNumber() const;

  /**
   * @brief Retrieves the Size of the packet.
   * @return The packet Size in bytes.
   */
  [[nodiscard]] uint16_t GetPacketSize() const;

  /**
   * @brief Constructs a `PacketHeader` object with specified values.
   * @param sid Stream ID.
   * @param reserved Reserved field.
   * @param marker_bit Marker bit.
   * @param sequence_number Sequence number.
   * @param packet_size Size of the packet in bytes.
   */
  PacketHeader(uint16_t sid, uint8_t reserved, bool marker_bit,
               uint8_t sequence_number, uint16_t packet_size);

  /**
   * @brief Constructs a `PacketHeader` object by reading from a bitstream.
   * @param reader Bitstream reader to extract packet header fields.
   */
  explicit PacketHeader(util::BitReader& reader);

  /**
   * @brief Writes the packet header fields to a bitstream.
   * @param writer Bitstream writer to output the packet header.
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Returns the length of the packet header in bits.
   * @return Fixed length of the packet header in bits.
   */
  [[nodiscard]] static uint64_t GetLength();
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_PACKET_HEADER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
