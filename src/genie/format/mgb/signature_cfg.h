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
    boost::optional<uint16_t> num_signatures;   //!< @brief
    std::vector<uint64_t> U_cluster_signature;  //!< @brief

    uint8_t U_signature_size;  //!< @brief

 public:
    /**
     * @brief
     * @param _U_cluster_signature_0
     * @param _U_signature_size
     */
    SignatureCfg(uint64_t _U_cluster_signature_0, uint8_t _U_signature_size);

    /**
     * @brief
     * @param _U_signature_size
     * @param multiple_signature_base
     * @param reader
     */
    SignatureCfg(uint8_t _U_signature_size, uint32_t multiple_signature_base, util::BitReader& reader);

    /**
     * @brief
     */
    virtual ~SignatureCfg() = default;

    /**
     * @brief
     * @param _U_cluster_signature
     */
    void addSignature(uint64_t _U_cluster_signature);

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
