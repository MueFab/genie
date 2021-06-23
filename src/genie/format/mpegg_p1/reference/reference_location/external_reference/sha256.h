/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_SHA256_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_SHA256_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <vector>
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
class Sha256 : public Checksum {
 private:
    std::array<uint64_t, 4> data;  //!< @brief

 public:
    /**
     * @brief
     */
    Sha256();

    /**
     * @brief
     * @param reader
     */
    explicit Sha256(util::BitReader& reader);

    /**
     * @brief
     * @param _data
     */
    explicit Sha256(std::vector<uint64_t>& _data);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const override;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_SHA256_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
