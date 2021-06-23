/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_BLOCK_HEADER_FLAGS_BLOCK_HEADER_FLAGS_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_BLOCK_HEADER_FLAGS_BLOCK_HEADER_FLAGS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/format/mpegg_p1/dataset/fields/block_header_flags/class_info/class_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class BlockConfig {
 public:
    /**
     * @brief
     */
    BlockConfig();

    /**
     * @brief
     * @param _block_header_flag
     * @param _mit_flag
     */
    BlockConfig(bool _block_header_flag, bool _mit_flag);

    /**
     * @brief
     * @param reader
     */
    void ReadBlockConfig(genie::util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    bool getBlockHeaderFlag() const;

    /**
     * @brief
     * @return
     */
    bool getMITFlag() const;

    /**
     * @brief
     * @return
     */
    uint8_t getNumClasses() const;

    /**
     * @brief
     * @return
     */
    const std::vector<ClassInfo>& getClassInfos() const;

    /**
     * @brief
     * @param _cls_info
     */
    void addClassInfo(ClassInfo&& _cls_info);

    /**
     * @brief
     * @param _cls_infos
     */
    void addClassInfos(std::vector<ClassInfo>& _cls_infos);

    /**
     * @brief
     * @param _cls_infos
     */
    void setClassInfos(std::vector<ClassInfo>&& _cls_infos);

    /**
     * @brief
     * @return
     */
    virtual uint64_t getBitLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    virtual void write(genie::util::BitWriter& bit_writer) const;

    /**
     * @brief
     * @param bit_writer
     */
    void writeClassInfos(genie::util::BitWriter& bit_writer) const;

 private:
    bool block_header_flag;  //!< @brief
    bool MIT_flag;           //!< @brief

    bool CC_mode_flag;         //!< @brief
    bool ordered_blocks_flag;  //!< @brief

    std::vector<ClassInfo> class_infos;  //!< @brief
    uint8_t num_classes;                 //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_BLOCK_HEADER_FLAGS_BLOCK_HEADER_FLAGS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
