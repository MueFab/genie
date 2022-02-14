/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_SEQUENCE_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_SEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <utility>
#include "genie/core/constants.h"
#include "genie/core/meta/sequence.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace reference {

/**
 * @brief
 */
class Sequence {
 private:
    std::string name;                       //!< @brief
    uint32_t sequence_length;               //!< @brief
    uint16_t sequence_id;                   //!< @brief
    genie::core::MPEGMinorVersion version;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const Sequence& other) const;

    /**
     * @brief
     * @param _name
     * @param length
     * @param id
     * @param _version
     */
    Sequence(std::string _name, uint32_t length, uint16_t id, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param s
     * @param _version
     */
    Sequence(genie::core::meta::Sequence s, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @return
     */
    const std::string& getName() const;

    /**
     * @brief
     * @return
     */
    uint32_t getLength() const;

    /**
     * @brief
     * @return
     */
    uint16_t getID() const;

    /**
     * @brief
     * @param reader
     * @param _version
     */
    Sequence(genie::util::BitReader& reader, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace reference
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_SEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
