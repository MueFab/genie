/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/fasta_ref.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/mpegg_ref.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/raw_ref.h"
#include "genie/format/mpegg_p1/reference/reference_location/reference_location.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class External : public ReferenceLocation {
 private:
    std::string ref_uri;                                    //!< @brief
    std::unique_ptr<ExternalReference> external_reference;  //!< @brief

 public:
    /**
     * @brief
     * @param reader
     * @param seq_count
     */
    explicit External(util::BitReader& reader, uint16_t seq_count);

    /**
     * @brief
     * @return
     */
    std::string getRefUri() const;

    /**
     * @brief
     * @return
     */
    Checksum::Algo getChecksumAlg() const;

    /**
     * @brief
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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
