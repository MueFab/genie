/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_U_ACCESS_UNIT_INFO_U_ACCESS_UNIT_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_U_ACCESS_UNIT_INFO_U_ACCESS_UNIT_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class UAccessUnitInfo {
 private:
    uint32_t num_U_clusters;           //!< @brief
    uint32_t multiple_signature_base;  //!< @brief
    uint8_t U_signature_size;          //!< @brief
    bool U_signature_constant_length;  //!< @brief
    uint8_t U_signature_length;        //!< @brief

 public:
    /**
     * @brief
     */
    UAccessUnitInfo();

    /**
     * @brief
     * @param _num_U_clusters
     */
    explicit UAccessUnitInfo(uint32_t _num_U_clusters);

    /**
     * @brief
     * @param reader
     */
    void readUAccessUnitInfo(genie::util::BitReader& reader);

    /**
     * @brief
     * @param base
     * @param size
     */
    void setMultipleSignature(uint32_t base, uint8_t size);

    /**
     * @brief
     * @param sign_length
     */
    void setConstantSignature(uint8_t sign_length);

    /**
     * @brief Get length in bit
     *
     * @return
     */
    uint64_t getBitLength() const;

    /**
     * @brief Write to bit_writer
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_U_ACCESS_UNIT_INFO_U_ACCESS_UNIT_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
