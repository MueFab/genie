/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "boost/optional/optional.hpp"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_parameterset {

/**
 * @brief
 */
class USignature {
 private:
    boost::optional<uint8_t> u_signature_length;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const USignature& other) const;

    /**
     * @brief
     */
    USignature();

    /**
     * @brief
     * @param reader
     */
    explicit USignature(genie::util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    bool isConstantLength() const;

    /**
     * @brief
     * @return
     */
    uint8_t getConstLength() const;

    /**
     * @brief
     * @param length
     */
    void setConstLength(uint8_t length);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace dataset_parameterset
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
