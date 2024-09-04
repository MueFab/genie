/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_SIGNATURE_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @brief
 */
class USignature {
 private:
    std::optional<uint8_t> const_length;  //!< @brief

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
     * @param _const_length
     */
    explicit USignature(uint8_t _const_length);

    /**
     * @brief
     * @param reader
     */
    explicit USignature(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool isConstLength() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getConstLength() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
