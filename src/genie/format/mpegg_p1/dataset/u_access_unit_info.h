/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_U_ACCESS_UNIT_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_U_ACCESS_UNIT_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
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
class UAccessUnitInfo {
 private:
    uint32_t num_U_access_units;  //!< @brief
    bool U_signature_flag;
    uint32_t multiple_signature_base;  //!< @brief
    uint8_t U_signature_size;          //!< @brief
    bool U_signature_constant_length;  //!< @brief
    uint8_t U_signature_length;        //!< @brief

    bool reserved_flag;

 public:
    /**
     * @brief
     */
    UAccessUnitInfo();

    /**
     * @brief
     * @param reader
     * @param fhd
     */
    UAccessUnitInfo(genie::util::BitReader& reader, FileHeader& fhd);

    /**
     *
     * @return
     */
    uint32_t getNumAccessUnits() const;

    /**
     *
     * @return
     */
    bool getUSignatureFlag() const;

    /**
     *
     */
    bool getUSignatureConstantLength() const;

    /**
     *
     * @return
     */
    uint8_t getUSignatureLength() const;

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
     * @brief Write to writer
     *
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_U_ACCESS_UNIT_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
