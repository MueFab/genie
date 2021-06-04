/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_AU_TYPE_CFG_H_
#define SRC_GENIE_FORMAT_MGB_AU_TYPE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "boost/optional/optional.hpp"
#include "genie/format/mgb/extended_au.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 *
 */
class AuTypeCfg {
 private:
    uint16_t sequence_ID;                     //!<
    uint64_t AU_start_position;               //!<
    uint64_t AU_end_position;                 //!<
    boost::optional<ExtendedAu> extended_AU;  //!<

    uint8_t posSize;  //!< internal

 public:
    /**
     *
     * @param _sequence_ID
     * @param _AU_start_position
     * @param _AU_end_position
     * @param posSize
     */
    AuTypeCfg(uint16_t _sequence_ID, uint64_t _AU_start_position, uint64_t _AU_end_position, uint8_t posSize);

    /**
     *
     * @param posSize
     */
    explicit AuTypeCfg(uint8_t posSize);

    /**
     *
     * @param _posSize
     * @param multiple_alignments
     * @param reader
     */
    AuTypeCfg(uint8_t _posSize, bool multiple_alignments, util::BitReader &reader);

    /**
     *
     */
    virtual ~AuTypeCfg() = default;

    /**
     *
     * @param _extended_AU
     */
    void setExtendedAu(ExtendedAu &&_extended_AU);

    /**
     *
     * @return
     */
    uint16_t getRefID() const;

    /**
     *
     * @return
     */
    uint64_t getStartPos() const;

    /**
     *
     * @return
     */
    uint64_t getEndPos() const;

    /**
     *
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
