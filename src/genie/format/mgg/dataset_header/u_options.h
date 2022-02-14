/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_OPTIONS_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "boost/optional/optional.hpp"
#include "genie/format/mgg/dataset_header/u_signature.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_header {

/**
 * @brief
 */
class UOptions {
 private:
    uint64_t reserved1;                       //!< @brief
    boost::optional<USignature> u_signature;  //!< @brief
    boost::optional<uint8_t> reserved2;       //!< @brief
    bool reserved3;                           //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const UOptions& other) const;

    /**
     * @brief
     * @param _reserved1
     * @param _reserved3
     */
    explicit UOptions(uint64_t _reserved1 = 0, bool _reserved3 = false);

    /**
     * @brief
     * @param reader
     */
    explicit UOptions(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    uint64_t getReserved1() const;

    /**
     * @brief
     * @return
     */
    bool getReserved3() const;

    /**
     * @brief
     * @return
     */
    bool hasReserved2() const;

    /**
     * @brief
     * @return
     */
    uint8_t getReserved2() const;

    /**
     * @brief
     * @return
     */
    bool hasSignature() const;

    /**
     * @brief
     * @return
     */
    const USignature& getSignature() const;

    /**
     * @brief
     * @param s
     */
    void addSignature(USignature s);

    /**
     * @brief
     * @param r
     */
    void addReserved2(uint8_t r);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace dataset_header
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
