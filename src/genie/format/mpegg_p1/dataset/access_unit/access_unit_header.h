/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_

#include <genie/core/constants.h>
#include <genie/core/parameter/data_unit.h>
#include <genie/core/record/class-type.h>
#include <genie/format/mgb/au_type_cfg.h>
#include <genie/format/mgb/mm_cfg.h>
#include <genie/format/mgb/ref_cfg.h>
#include <genie/format/mpegg_p1/dataset/dataset_header.h>
#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/dataset/access_unit/signature_cfg.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class AccessUnitHeader {
 private:
    /* ----- access_unit_header ----- */
    uint32_t ID;                      //!< @brief
    uint8_t num_blocks;               //!< @brief
    uint8_t parameter_set_ID;         //!< @brief
    core::record::ClassType au_type;  //!< @brief
    uint32_t reads_count;             //!< @brief

    std::unique_ptr<format::mgb::MmCfg> mm_cfg;                 //!< @brief
    std::unique_ptr<format::mgb::RefCfg> ref_cfg;               //!< @brief
    std::unique_ptr<format::mgb::AuTypeCfg> au_type_cfg;        //!< @brief
    std::vector<std::unique_ptr<SignatureCfg>> signature_cfgs;  //!< @brief

    /* ----- internal ----- */
    bool MIT_flag;  //!< @brief

 public:
    /**
     * @brief
     */
    AccessUnitHeader();

    /**
     * @brief
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     * @param dhd
     */
    explicit AccessUnitHeader(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length,
                              DatasetHeader& dhd);

    /**
     * @brief
     * @return
     */
    uint32_t getID() const;

    /**
     * @brief
     * @return
     */
    uint8_t getNumBlocks() const;

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     * @param zero_length
     */
    void write(util::BitWriter& writer, bool zero_length = false) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
