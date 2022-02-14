/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_H_
#define SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/extended_au.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/format/mgg/dataset_header.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/master_index_table/aligned_au_index.h"
#include "genie/format/mgg/master_index_table/unaligned_au_index.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class MasterIndexTable : public GenInfo {
 private:
    std::vector<std::vector<std::vector<master_index_table::AlignedAUIndex>>> aligned_aus;  //!< @brief
    std::vector<master_index_table::UnalignedAUIndex> unaligned_aus;                        //!< @brief

 public:
    /**
     * @brief
     * @param class_index
     * @param desc_index
     * @param isUnaligned
     * @param total_size
     * @return
     */
    std::vector<uint64_t> getDescriptorStreamOffsets(uint8_t class_index, uint8_t desc_index, bool isUnaligned,
                                                     uint64_t total_size) const;

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @return
     */
    const std::vector<std::vector<std::vector<master_index_table::AlignedAUIndex>>>& getAlignedAUs() const;

    /**
     * @brief
     * @return
     */
    const std::vector<master_index_table::UnalignedAUIndex>& getUnalignedAUs() const;

    /**
     * @brief
     * @param seq_count
     * @param num_classes
     */
    MasterIndexTable(uint16_t seq_count, uint8_t num_classes);

    /**
     * @brief
     * @param reader
     * @param hdr
     */
    MasterIndexTable(util::BitReader& reader, const DatasetHeader& hdr);

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
