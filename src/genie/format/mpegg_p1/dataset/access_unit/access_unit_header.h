/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include <genie/core/constants.h>
#include <genie/core/record/class-type.h>
#include <genie/core/parameter/data_unit.h>
#include <genie/format/mgb/au_type_cfg.h>
#include <genie/format/mgb/mm_cfg.h>
#include <genie/format/mgb/ref_cfg.h>
#include "genie/format/mpegg_p1/dataset/access_unit/signature_cfg.h"
#include <genie/format/mpegg_p1/dataset/dataset_header.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

class AccessUnitHeader{
 private:
    /* ----- access_unit_header ----- */
    uint32_t ID;
    uint8_t num_blocks;
    uint8_t parameter_set_ID;
    core::record::ClassType au_type;
    uint32_t reads_count;

    std::unique_ptr<format::mgb::MmCfg> mm_cfg;
    std::unique_ptr<format::mgb::RefCfg> ref_cfg;
    std::unique_ptr<format::mgb::AuTypeCfg> au_type_cfg;
    std::vector<std::unique_ptr<SignatureCfg>> signature_cfgs;

    /* ----- internal ----- */
    bool MIT_flag;
//    bool U_signature_flag;

 public:

    /**
     *
     */
    AccessUnitHeader();

    /**
     *
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    explicit AccessUnitHeader(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length,
                              DatasetHeader& dhd);

    /**
     *
     * @return
     */
    uint32_t getID() const;

    /**
     *
     * @return
     */
    uint8_t getNumBlocks() const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param writer
     */
    void write(util::BitWriter& writer, bool zero_length=false) const;
};
}
}
}
#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_
