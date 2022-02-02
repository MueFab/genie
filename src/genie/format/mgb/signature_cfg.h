/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_SIGNATURE_CFG_H_
#define SRC_GENIE_FORMAT_MGB_SIGNATURE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * @brief
 */
class SignatureCfg {
 private:
    std::vector<uint64_t> U_cluster_signature;        //!< @brief
    std::vector<uint8_t> U_cluster_signature_length;  //!< @brief

    boost::optional<uint8_t> U_signature_size;  //!< @brief
    uint8_t base_bits;
 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const SignatureCfg& other) const;

    /**
     * @brief
     */
    explicit SignatureCfg(uint8_t _base_bit) : base_bits(_base_bit){
    }

    /**
     * @brief
     * @param reader
     * @param _U_signature_size
     */
    SignatureCfg(util::BitReader& reader, uint8_t _U_signature_size, uint8_t _base_bits);

    /**
     * @brief
     */
    virtual ~SignatureCfg() = default;

    /**
     * @brief
     * @param _U_cluster_signature
     * @param length
     */
    void addSignature(uint64_t _U_cluster_signature, uint8_t length);

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_SIGNATURE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
