/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_REF_CFG_H_
#define SRC_GENIE_FORMAT_MGB_REF_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * ISO 23092-2 Section 3.4.1.1 table 19 lines 11 to 15
 */
class RefCfg {
 private:
    uint16_t ref_sequence_ID;     //!< Line 12
    uint64_t ref_start_position;  //!< Line 13
    uint64_t ref_end_position;    //!< Line 14

    uint8_t posSize;  //!< internal

 public:
    /**
     *
     * @return
     */
    uint16_t getSeqID() const;

    /**
     *
     * @return
     */
    uint64_t getStart() const;

    /**
     *
     * @return
     */
    uint64_t getEnd() const;

    /**
     *
     * @param _ref_sequence_ID
     * @param _ref_start_position
     * @param _ref_end_position
     * @param _posSize
     */
    RefCfg(uint16_t _ref_sequence_ID, uint64_t _ref_start_position, uint64_t _ref_end_position, uint8_t _posSize);

    /**
     *
     * @param _posSize
     */
    explicit RefCfg(uint8_t _posSize);

    /**
     *
     * @param _posSize
     * @param reader
     */
    RefCfg(uint8_t _posSize, util::BitReader &reader);

    /**
     *
     */
    virtual ~RefCfg() = default;

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

#endif  // SRC_GENIE_FORMAT_MGB_REF_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
