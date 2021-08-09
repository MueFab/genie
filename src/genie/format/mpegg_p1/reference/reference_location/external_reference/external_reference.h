/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_EXTERNAL_REFERENCE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_EXTERNAL_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/checksum.h"
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
class ExternalReference {
 public:
    /**
     * @brief
     */
    enum class Type : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2, UNKNOWN = 3 };

    /**
     * @brief
     */
    ExternalReference();

    /**
     * @brief
     * @param _reference_type
     */
    explicit ExternalReference(Type _reference_type);

    /**
     * @brief
     */
    virtual ~ExternalReference() = default;

    /**
     *
     * @return
     */
    virtual std::unique_ptr<ExternalReference> clone() const = 0;

    /**
     * @brief
     * @return
     */
    virtual Checksum::Algo getChecksumAlg() const;

    /**
     * @brief
     * @return
     */
    Type getType() const;

    /**
     * @brief
     * @return
     */
    virtual uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    virtual void write(genie::util::BitWriter& writer) const;

 private:
    Type reference_type;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_EXTERNAL_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
