#ifndef GENIE_SIGNATURE_CFG_H
#define GENIE_SIGNATURE_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "util/make_unique.h"
#include "util/bitwriter.h"
#include "util/bitreader.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {

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
    SignatureCfg(uint8_t _U_signature_size, uint32_t multiple_signature_base, util::BitReader& reader) : U_signature_size(_U_signature_size){
        if(multiple_signature_base != 0) {
            U_cluster_signature.emplace_back(reader.read(U_signature_size));
            if(U_cluster_signature[0] != (1u << U_signature_size) - 1u) {
                for(size_t i=1; i < multiple_signature_base; ++i) {
                    U_cluster_signature.emplace_back(reader.read(U_signature_size));
                }
            } else {
                num_signatures = util::make_unique<uint16_t >(reader.read(16));
                for(size_t i=0; i < *num_signatures; ++i) {
                    U_cluster_signature.emplace_back(reader.read(U_signature_size));
                }
            }
        }
    }
    virtual ~SignatureCfg() = default;

    void addSignature(uint64_t _U_cluster_signature);
    virtual void write(util::BitWriter &writer);
};
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SIGNATURE_CFG_H
