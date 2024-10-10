/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <vector>
#include "genie/format/mgb/access_unit_header.h"
#include "genie/format/mgb/block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class AccessUnit : public core::parameter::DataUnit {
 public:
    /**
     * @brief
     */
    void debugPrint(const core::parameter::EncodingSet &ps) const;

    /**
     * @brief
     * @param parameterSets
     * @param bitReader
     * @param lazyPayload
     */
    explicit AccessUnit(const std::map<size_t, core::parameter::EncodingSet> &parameterSets, util::BitReader &bitReader,
                        bool lazyPayload = false);

    /**
     * @brief
     * @param bitReader
     */
    void loadPayload(util::BitReader &bitReader);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t getPayloadSize() const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief
     * @param _access_unit_ID
     * @param _parameter_set_ID
     * @param _au_type
     * @param _reads_count
     * @param dataset_type
     * @param posSize
     * @param signatureFlag
     * @param alphabet
     */
    AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
               uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, bool signatureFlag,
               core::AlphabetID alphabet);

    /**
     * @brief
     * @param block
     */
    void addBlock(Block block);

    /**
     * @brief
     * @return
     */
    std::vector<Block> &getBlocks();

    /**
     * @brief
     * @return
     */
    AUHeader &getHeader();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const AUHeader &getHeader() const;

    /**
     * @brief
     * @param output
     */
    void print_debug(std::ostream &output, uint8_t, uint8_t) const override;

    /**
     * @brief
     * @param h
     * @param b
     */
    AccessUnit(AUHeader h, std::vector<Block> b);

 private:
    AUHeader header;  //!< @brief

    std::vector<Block> blocks;  //!< @brief

    size_t payloadbytes;  //!< @brief
    size_t qv_payloads;   //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
