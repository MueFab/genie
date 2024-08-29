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

namespace genie::format::mgb {

/**
 * @brief
 */
class RefCfg {
 private:
    uint16_t ref_sequence_ID;     //!< @brief
    uint64_t ref_start_position;  //!< @brief
    uint64_t ref_end_position;    //!< @brief

    uint8_t posSize;  //!< @brief internal

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const RefCfg &other) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getSeqID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t getStart() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t getEnd() const;

    /**
     * @brief
     * @param _ref_sequence_ID
     * @param _ref_start_position
     * @param _ref_end_position
     * @param _posSize
     */
    RefCfg(uint16_t _ref_sequence_ID, uint64_t _ref_start_position, uint64_t _ref_end_position, uint8_t _posSize);

    /**
     * @brief
     * @param _posSize
     */
    explicit RefCfg(uint8_t _posSize);

    /**
     * @brief
     * @param _posSize
     * @param reader
     */
    RefCfg(uint8_t _posSize, util::BitReader &reader);

    /**
     * @brief
     */
    virtual ~RefCfg() = default;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_REF_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
