/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#if 0
#include "genie/format/mpegg_p1/dataset/access_unit/signature_cfg.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

SignatureCfg::SignatureCfg(util::BitReader& reader, DatasetHeader& dhd) {
    auto U_signature_constant_length = dhd.getUAccessUnitInfo().getUSignatureConstantLength();

    if (U_signature_constant_length) {
        U_signature_size = dhd.getUAccessUnitInfo().getUSignatureLength();
    } else {
        U_signature_size = 0;
    }

    auto num_signatures = reader.read<uint16_t>();
    for (auto i = 0; i < num_signatures; i++) {
        if (!U_signature_constant_length) {
            U_signature_length.emplace_back(reader.read<uint8_t>());
            U_cluster_signature.emplace_back(reader.read<uint64_t>(U_signature_length[i]));
        } else {
            U_cluster_signature.emplace_back(reader.read<uint64_t>(U_signature_size));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

// void SignatureCfg::addSignature(uint64_t _U_cluster_signature) {
//     U_cluster_signature.push_back(_U_cluster_signature);
//     if (num_signatures) {
//         ++*num_signatures;
//     }
// }

bool SignatureCfg::getUSignatureConstantLength() const {
    //    UTILS_DIE_IF(U_cluster_signature.empty(),
    //                 "There is no U_cluster_signature, cannot determine U_signature_constant_length");
    //    return !U_signature_length.empty();
    return U_signature_size != 0;
}

// ---------------------------------------------------------------------------------------------------------------------

void SignatureCfg::write(util::BitWriter& writer) const {
    auto num_signatures = U_cluster_signature.size();
    bool U_signature_constant_length = getUSignatureConstantLength();

    /// num_signatures u(16)
    writer.write(num_signatures, 16);

    for (size_t i = 0; i < num_signatures; i++) {
        if (!U_signature_constant_length) {
            writer.write(U_signature_length[i], 8);
            writer.write(U_cluster_signature[i], U_signature_length[i]);
        } else {
            writer.write(U_cluster_signature[i], U_signature_size);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
#endif