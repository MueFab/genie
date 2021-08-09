/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_FASTA_REF_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_FASTA_REF_H_

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

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/checksum.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class FastaReference : public ExternalReference {
 private:
    std::vector<std::unique_ptr<Checksum>> checksums;  //!< @brief

 public:
    /**
     * @brief
     */
    FastaReference();

    /**
     *
     * @param _checksums
     * @return
     */
    FastaReference(const std::vector<std::unique_ptr<Checksum>>& _checksums);

    /**
     * @brief
     * @param reader
     * @param checksum_alg
     * @param seq_count
     */
    FastaReference(util::BitReader& reader, FileHeader& fhd, Checksum::Algo checksum_alg, uint16_t seq_count);

    /**
     *
     * @return
     */
    std::unique_ptr<ExternalReference> clone() const override;

    /**
     * @brief
     * @return
     */
    Checksum::Algo getChecksumAlg() const override;

    /**
     *
     * @return
     */
    uint64_t getLength() const override;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_FASTA_REF_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
