/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_MPEGG_REF_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_MPEGG_REF_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/checksum.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h"
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
class MpegReference : public ExternalReference {
 private:
    uint8_t dataset_group_ID;  //!< @brief
    uint16_t dataset_ID;       //!< @brief

    std::unique_ptr<Checksum> ref_checksum;  //!< @brief

 public:
    /**
     * @brief
     * @param _dataset_group_ID
     * @param _dataset_ID
     * @param _ref_checksum
     */
    MpegReference(uint8_t _dataset_group_ID, uint16_t _dataset_ID, std::unique_ptr<Checksum> _ref_checksum);

    /**
     * @brief
     * @param reader
     * @param checksum_alg
     */
    MpegReference(util::BitReader& reader, Checksum::Algo checksum_alg);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    // TODO(Yeremia): Is it needed?
    //    /**
    //     *
    //     * @param _checksum
    //     */
    //    void addChecksum(Checksum &&_checksum);

    /**
     * @brief
     * @return
     */
    Checksum::Algo getChecksumAlg() const override;

    /**
     * @brief
     * @return
     */
    uint64_t getLength();

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_MPEGG_REF_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
