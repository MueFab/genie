#ifndef GENIE_SIGNATURE_CFG_H
#define GENIE_SIGNATURE_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "util/bitreader.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {

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
    SignatureCfg(uint8_t _U_signature_size, uint32_t multiple_signature_base, util::BitReader& reader);
    virtual ~SignatureCfg() = default;

    void addSignature(uint64_t _U_cluster_signature);
    virtual void write(util::BitWriter& writer);
};
}  // namespace mpegg_p2
}  // namespace genie

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SIGNATURE_CFG_H
