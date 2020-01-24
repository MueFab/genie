#ifndef GENIE_SIGNATURE_CFG_H
#define GENIE_SIGNATURE_CFG_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <boost/optional/optional.hpp>
#include <memory>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

class SignatureCfg {
   private:
    boost::optional<uint16_t> num_signatures;
    std::vector<uint64_t> U_cluster_signature;

    uint8_t U_signature_size;

   public:
    SignatureCfg(uint64_t _U_cluster_signature_0, uint8_t _U_signature_size);
    SignatureCfg(uint8_t _U_signature_size, uint32_t multiple_signature_base, util::BitReader& reader);
    virtual ~SignatureCfg() = default;

    void addSignature(uint64_t _U_cluster_signature);
    virtual void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SIGNATURE_CFG_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------