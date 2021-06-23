/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_RAW_REF_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_RAW_REF_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/checksum.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class RawReference : public ExternalReference {
 private:
    std::vector<Checksum> checksums;  //!< @brief

 public:
    /**
     * @brief
     */
    RawReference();

    /**
     * @brief
     * @param reader
     * @param checksum_alg
     * @param seq_count
     */
    RawReference(util::BitReader& reader, Checksum::Algo checksum_alg, uint16_t seq_count);

    /**
     * @brief
     * @param _checksums
     */
    explicit RawReference(std::vector<Checksum>&& _checksums);

    /**
     * @brief
     * @return
     */
    Checksum::Algo getChecksumAlg() const override;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_RAW_REF_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
