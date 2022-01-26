/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_MD5_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_MD5_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <list>
#include <memory>
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
class Md5 : public Checksum {
 private:
    std::list<uint64_t> data;  //!< @brief

 public:
    /**
     * @brief Default constructor
     */
    Md5();

    /**
     * @brief Read MD5 checksum from reader
     * @param reader
     */
    explicit Md5(util::BitReader& reader);

    /**
     *
     * @return
     */
    std::unique_ptr<Checksum> clone() const override;

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_MD5_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
