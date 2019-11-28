#include "signature_cfg.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
SignatureCfg::SignatureCfg(uint64_t _U_cluster_signature_0, uint8_t _U_signature_size)
    : num_signatures(nullptr), U_cluster_signature(1, _U_cluster_signature_0), U_signature_size(_U_signature_size) {
    if (U_cluster_signature[0] == (1u << U_signature_size) - 1) {
        num_signatures = make_unique<uint16_t>(1);
    }
}

// -----------------------------------------------------------------------------------------------------------------

void SignatureCfg::addSignature(uint64_t _U_cluster_signature) {
    U_cluster_signature.push_back(_U_cluster_signature);
    if (num_signatures) {
        ++*num_signatures;
    }
}

// -----------------------------------------------------------------------------------------------------------------

void SignatureCfg::write(util::BitWriter *writer) {
    size_t i = 0;

    writer->write(U_cluster_signature[0], U_signature_size);  // todo: size;
    if (num_signatures) {
        writer->write(*num_signatures, 16);
        i = 1;
    }
    for (; i < U_cluster_signature.size(); ++i) {
        writer->write(U_cluster_signature[i], U_signature_size);  // todo: size;
    }
}
}  // namespace format
