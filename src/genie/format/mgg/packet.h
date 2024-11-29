/**
 * Copyright 2018-2024 The Genie Authors.
 * @file packet.h
 * @copyright This file is part of Genie.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `Packet` class for handling MPEG-G packets.
 *
 * The `Packet` class provides a representation of a packet within the MPEG-G
 * format, including its header and associated data. This class is used for
 * encapsulating packet-level metadata and raw data payloads, ensuring proper
 * reading and writing operations are handled correctly when processing MPEG-G
 * streams.
 *
 * The packet structure in MPEG-G is crucial for maintaining the logical
 * separation of data and metadata, allowing efficient parsing, transport, and
 * storage of genomic data.
 */

#ifndef SRC_GENIE_FORMAT_MGG_PACKET_H_
#define SRC_GENIE_FORMAT_MGG_PACKET_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/format/mgg/box.h"
#include "genie/format/mgg/packet_header.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class Packet
 * @brief Represents a single packet within an MPEG-G container.
 *
 * The `Packet` class encapsulates a packet's header (`PacketHeader`) and its
 * associated data payload. This class allows for reading and writing packets to
 * a bitstream, making it a core component in the MPEG-G file format's handling
 * of genomic data.
 *
 * ### Packet Structure:
 * A packet consists of:
 * 1. **Packet Header** - Encodes metadata such as stream ID, sequence number,
 * and packet Size.
 * 2. **Data** - Raw data payload associated with the packet, stored as a
 * string.
 */
class Packet final : public Box {
  PacketHeader header_;  //!< @brief Header containing packet metadata (stream
                         //!< ID, sequence number, etc.)
  std::string data_;     //!< @brief The raw data payload of the packet.

 public:
  /**
   * @brief Constructs a `Packet` object with the specified header and data.
   * @param header Header containing packet metadata.
   * @param data The raw data payload for the packet.
   */
  Packet(PacketHeader header, std::string data);

  /**
   * @brief Constructs a `Packet` object by reading from a bitstream.
   * @param reader Bitstream reader to extract packet header and data.
   */
  explicit Packet(util::BitReader& reader);

  /**
   * @brief Writes the packet header and data to a bitstream.
   * @param writer Bitstream writer for serializing the packet.
   */
  void Write(util::BitWriter& writer) const override;

  /**
   * @brief Retrieves the header of the packet.
   * @return Constant reference to the packet header.
   */
  [[nodiscard]] const PacketHeader& GetHeader() const;

  /**
   * @brief Retrieves the data payload of the packet.
   * @return Constant reference to the data payload.
   */
  [[nodiscard]] const std::string& GetData() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_PACKET_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
