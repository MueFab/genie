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

 public:
    /**
     * @brief
     * @param _U_cluster_signature_0
     * @param _U_signature_size
     */
    SignatureCfg() = default;

    /**
     * @brief
     * @param _U_signature_size
     * @param multiple_signature_base
     * @param reader
     */
    SignatureCfg(util::BitReader& reader, uint8_t _U_signature_size)
        : U_signature_size(_U_signature_size ? boost::optional<uint8_t>(_U_signature_size)
                                             : boost::optional<uint8_t>(boost::none)) {
        auto num_signatures = reader.read<uint16_t>();
        for (uint16_t i = 0; i < num_signatures; ++i) {
            size_t len = 0;
            if (U_signature_size != boost::none) {
                len = *U_signature_size;
            } else {
                len = reader.read<uint8_t>(8);
                U_cluster_signature_length.emplace_back(len);
            }
            const size_t ACTGN_BITS = 3;
            U_cluster_signature.emplace_back(reader.read<uint64_t>(ACTGN_BITS * len));
        }
    }

    /**
     * @brief
     */
    virtual ~SignatureCfg() = default;

    /**
     * @brief
     * @param _U_cluster_signature
     */
    void addSignature(uint64_t _U_cluster_signature, uint8_t length) {
        if (U_cluster_signature.empty()) {
            U_signature_size = length;
        } else {
            if (length != U_signature_size) {
                U_signature_size = boost::none;
            }
        }
        U_cluster_signature_length.emplace_back(length);
        U_cluster_signature.emplace_back(_U_cluster_signature);
    }

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const {
        const size_t ACTGN_BITS = 3;
        writer.write(U_cluster_signature.size(), 16);
        for (size_t i = 0; i < U_cluster_signature.size(); ++i) {
            if (U_signature_size != boost::none) {
                writer.write(U_cluster_signature[i], ACTGN_BITS * *U_signature_size);
            } else {
                writer.write(U_cluster_signature[i], ACTGN_BITS * U_cluster_signature_length[i]);
            }
        }
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_SIGNATURE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
