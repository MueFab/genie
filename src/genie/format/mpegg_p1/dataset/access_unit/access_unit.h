/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/format/mpegg_p1/dataset/dataset_header.h>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/dataset/access_unit/access_unit_header.h"
#include "genie/format/mpegg_p1/dataset/access_unit/block.h"
#include "genie/format/mpegg_p1/dataset/class_description.h"
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
class AUInformation {
 private:
    std::list<uint8_t> AU_information_value;  //!< @brief
    std::string minor_version;                //!< @brief
    uint8_t dataset_group_ID;                 //!< @brief
    uint16_t dataset_ID;                      //!< @brief

 public:
    /**
     * @brief
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    AUInformation(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

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

/**
 * @brief
 */
class AUProtection {
 private:
    std::list<uint8_t> AU_protection_value;  //!< @brief
    std::string minor_version;               //!< @brief
    uint8_t dataset_group_ID;                //!< @brief
    uint16_t dataset_ID;                     //!< @brief

 public:
    /**
     * @brief
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    AUProtection(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

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

/**
 * @brief
 */
class AccessUnit {
 private:
    AccessUnitHeader header;

    /* ----- internal ----- */
    bool block_header_flag;  //!< @brief From Dataset/BlockHeaderFlags/block_header_flag

    /* ----- access_unit ----- */

    std::unique_ptr<AUInformation> AU_information;  //!< @brief
    std::unique_ptr<AUProtection> AU_protection;    //!< @brief

    std::list<Block> blocks;  //!< @brief

 public:
    /**
     * @brief
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     * @param dhd
     */
    explicit AccessUnit(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length, DatasetHeader& dhd);

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
