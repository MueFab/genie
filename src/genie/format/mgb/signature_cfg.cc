/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/signature_cfg.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

bool SignatureCfg::operator==(const SignatureCfg& other) const {
    return U_cluster_signature == other.U_cluster_signature &&
           U_cluster_signature_length == other.U_cluster_signature_length && U_signature_size == other.U_signature_size;
}

// ---------------------------------------------------------------------------------------------------------------------

SignatureCfg::SignatureCfg(util::BitReader& reader, uint8_t _U_signature_size)
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

// ---------------------------------------------------------------------------------------------------------------------

void SignatureCfg::addSignature(uint64_t _U_cluster_signature, uint8_t length) {
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

// ---------------------------------------------------------------------------------------------------------------------

void SignatureCfg::write(util::BitWriter& writer) const {
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

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
