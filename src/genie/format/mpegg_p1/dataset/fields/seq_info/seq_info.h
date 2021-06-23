/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_SEQ_INFO_SEQ_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_SEQ_INFO_SEQ_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class SequenceConfig {
 private:
    uint8_t reference_ID;              //!< @brief
    std::vector<uint16_t> seq_IDs;     //!< @brief
    std::vector<uint16_t> seq_blocks;  //!< @brief

    std::vector<uint32_t> thress;  //!< @brief

 public:
    /**
     * @brief
     */
    SequenceConfig();

    /**
     * @brief
     * @param _ref_ID
     */
    explicit SequenceConfig(uint8_t _ref_ID);

    /**
     * @brief
     * @param reader
     */
    void ReadSequenceConfig(genie::util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    uint64_t getBitLength() const;

    /**
     * @brief
     * @return
     */
    bool anySeq() const;

    /**
     * @brief
     * @return
     */
    bool isValid() const;

    /**
     * @brief
     * @param _ref_ID
     */
    void setRefID(uint8_t _ref_ID);

    /**
     * @brief
     * @param seq_ID
     * @param seq_block
     */
    void addEntry(uint16_t seq_ID, uint16_t seq_block);

    /**
     * @brief
     * @param _seq_IDs
     * @param _seq_blocks
     */
    void addEntries(std::vector<uint16_t>& _seq_IDs, std::vector<uint16_t>& _seq_blocks);

    /**
     * @brief
     * @return
     */
    uint16_t getSeqCount() const;

    /**
     * @brief
     * @return
     */
    const std::vector<uint16_t>& getSeqBlocks() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @param bit_writer
     */
    void writeThres(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_SEQ_INFO_SEQ_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
