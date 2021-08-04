/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <memory>
#include "genie/util/make-unique.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/format/mpegg_p1/dataset/class_description.h"

#include <genie/format/mpegg_p1/dataset/dataset_header.h>
#include "genie/format/mpegg_p1/dataset/access_unit/access_unit_header.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class-type.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/format/mpegg_p1/dataset/access_unit/block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class AUInformation {
 private:
    std::vector<uint8_t> AU_information_value;  //!< @brief

 public:
    /**
     * @brief
     */
    AUInformation();

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

/**
 * @brief
 */
class AUProtection {
 private:
    std::vector<uint8_t> AU_protection_value;  //!< @brief

 public:
    /**
     * @brief
     */
    AUProtection();

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

/**
 * @brief
 */
class AccessUnit {
 private:

    AccessUnitHeader header;

    /* ----- internal ----- */
    bool block_header_flag;  //!< @brief From Dataset/BlockHeaderFlags/block_header_flag
    // already inside ref_cfg
    // uint8_t posSize;
    // already inside signature_config
    // uint8_t U_signature_size;

    /* ----- access_unit ----- */

    std::unique_ptr<AUInformation> AU_information;  //!< @brief
    std::unique_ptr<AUProtection> AU_protection;    //!< @brief

    std::list<Block> blocks;  //!< @brief

 public:
//    /**
//     * @brief
//     * @param _access_unit_ID
//     * @param _parameter_set_ID
//     * @param _au_type
//     * @param _reads_count
//     * @param dataset_type
//     * @param posSize
//     * @param signatureSize
//     * @param multiple_signature_base
//     */
//    AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
//               uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, uint8_t signatureSize,
//               uint32_t multiple_signature_base);
//
//    /**
//     * @brief
//     * @param au
//     */
//    explicit AccessUnit(format::mgb::AccessUnit&& au);

    /**
     *
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    explicit AccessUnit(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length,
                        DatasetHeader& dhd);

    /**
     * @brief
     * @return
     */
    uint64_t getHeaderLength() const;

    /**
     * @brief
     * @param writer
     * @param write_dummy_length
     */
    void writeHeader(util::BitWriter& writer, bool write_dummy_length = false) const;

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
