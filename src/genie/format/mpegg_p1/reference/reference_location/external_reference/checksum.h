/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_CHECKSUM_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_CHECKSUM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Checksum {
 public:
    /**
     * @brief
     */
    enum class Algo : uint8_t { MD5 = 0, SHA256 = 1, UNKNOWN = 2 };

 private:
    Algo checksum_alg;  //!< @brief

 public:
    /**
     * @brief
     */
    Checksum();

    /**
     * @brief
     * @param _algo
     */
    explicit Checksum(Algo _algo);

    /**
     *
     */
    virtual ~Checksum() = default;

    /**
     *
     * @return
     */
    virtual std::unique_ptr<Checksum> clone() const = 0;

    /**
     * @brief
     * @return
     */
    Algo getType() const;

    /**
     * @brief
     * @return
     */
    virtual uint64_t getLength() const = 0;

    /**
     * @brief
     * @param bit_writer
     */
    virtual void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_CHECKSUM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
