/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_PACKET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_PACKET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mpegg_p1/packet_header.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Packet {
 private:
    PacketHeader header;  //!< @brief
    std::string data;     //!< @brief

 public:
    /**
     * @brief
     * @param _header
     * @param _data
     */
    Packet(PacketHeader _header, std::string _data);

    /**
     * @brief
     * @param reader
     */
    explicit Packet(util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer);

    /**
     * @brief
     * @return
     */
    const PacketHeader& getHeader() const;

    /**
     * @brief
     * @return
     */
    const std::string& getData() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_PACKET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
