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
#include <vector>
#include "genie/format/mpegg_p1/dataset/class_description.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/fasta_ref.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/mpegg_ref.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/raw_ref.h"
#include "genie/format/mpegg_p1/reference/reference_location/reference_location.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class External : public ReferenceLocation {
 private:
    std::string ref_uri;  //!< @brief
    Checksum::Algo checksum_alg;
    std::unique_ptr<ExternalReference> external_reference;  //!< @brief

 public:
    External();

    /**
     * @brief
     * @param other
     */
    explicit External(const genie::format::mpegg_p1::External& other);

    /**
     * @brief
     * @param reader
     * @param fhd
     * @param seq_count
     */
    External(util::BitReader& reader, FileHeader& fhd, uint16_t seq_count);

    /**
     * @brief
     * @param _ref_uri
     * @param _checksum_alg
     * @param _ext_ref
     */
    External(std::string& _ref_uri, Checksum::Algo _checksum_alg, std::unique_ptr<ExternalReference> _ext_ref);

    /**
     * @brief
     * @param other
     * @return
     */
    External& operator=(const External& other);

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
     *
     * @return
     */
    ExternalReference& getExternalRef() const;

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
