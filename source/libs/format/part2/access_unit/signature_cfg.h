#ifndef GENIE_SIGNATURE_CFG_H
#define GENIE_SIGNATURE_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "../make_unique.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
class BitWriter;

/**
 * ISO 23092-2 Section 3.4.1.1 table 19 lines 26 to 39
 */
class SignatureCfg {
   private:
    std::unique_ptr<uint16_t> num_signatures;   //!< : 16; Line 33
    std::vector<uint64_t> U_cluster_signature;  //!< Lines 29 and 25 joined

    uint8_t U_signature_size;  //!< Internal
   public:
    SignatureCfg(uint64_t _U_cluster_signature_0, uint8_t _U_signature_size);

    void addSignature(uint64_t _U_cluster_signature);

    virtual void write(BitWriter *writer);
};
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SIGNATURE_CFG_H
