/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_parameterset {

/**
 * @brief
 */
class USignature {
 private:
    std::optional<uint8_t> u_signature_length;  //!< @brief

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
    [[nodiscard]] bool isConstantLength() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getConstLength() const;

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

}  // namespace genie::format::mgg::dataset_parameterset

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
