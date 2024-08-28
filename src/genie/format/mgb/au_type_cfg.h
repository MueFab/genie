/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_AU_TYPE_CFG_H_
#define SRC_GENIE_FORMAT_MGB_AU_TYPE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <optional>
#include "genie/format/mgb/extended_au.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * @brief
 */
class AuTypeCfg {
 private:
    uint16_t sequence_ID;                     //!< @brief
    uint64_t AU_start_position;               //!< @brief
    uint64_t AU_end_position;                 //!< @brief
    std::optional<ExtendedAu> extended_AU;  //!< @brief

    uint8_t posSize;  //!< @brief internal

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const AuTypeCfg &other) const;

    /**
     * @brief
     * @param _sequence_ID
     * @param _AU_start_position
     * @param _AU_end_position
     * @param posSize
     */
    AuTypeCfg(uint16_t _sequence_ID, uint64_t _AU_start_position, uint64_t _AU_end_position, uint8_t posSize);

    /**
     * @brief
     * @param posSize
     */
    explicit AuTypeCfg(uint8_t posSize);

    /**
     * @brief
     * @param _posSize
     * @param multiple_alignments
     * @param reader
     */
    AuTypeCfg(uint8_t _posSize, bool multiple_alignments, util::BitReader &reader);

    /**
     * @brief
     */
    virtual ~AuTypeCfg() = default;

    /**
     * @brief
     * @param _extended_AU
     */
    void setExtendedAu(ExtendedAu &&_extended_AU);

    /**
     * @brief
     * @return
     */
    uint16_t getRefID() const;

    /**
     * @brief
     * @return
     */
    uint64_t getStartPos() const;

    /**
     * @brief
     * @return
     */
    uint64_t getEndPos() const;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_AU_TYPE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
