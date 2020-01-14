#ifndef GENIE_PARAMETER_SET_CRPS_H
#define GENIE_PARAMETER_SET_CRPS_H

// -----------------------------------------------------------------------------------------------------------------

#include <util/make_unique.h>
#include <cstdint>
#include <memory>

#include "crps_info.h"
#include "util/bitreader.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
/**
 * ISO 23092-2 Section 3.3.2.3 table 16
 */
class ParameterSetCrps {
   public:
    /**
     * ISO 23092-2 Section 3.3.2.3 table 17
     */
    enum class CrAlgId : uint8_t {
        RESERVED = 0,
        REF_TRANSFORM = 1,
        PUSH_IN = 2,
        LOCAL_ASSEMBLY = 3,
        GLOBAL_ASSEMBLY = 4
    };

   private:
    CrAlgId cr_alg_ID;                    //!<< : 8; Line 2
    std::unique_ptr<CrpsInfo> crps_info;  //!<< Lines 3 to 6

   public:
    explicit ParameterSetCrps(CrAlgId _cr_alg_ID);

    explicit ParameterSetCrps(util::BitReader &reader) {
        cr_alg_ID = CrAlgId(reader.read(8));
        if (cr_alg_ID == CrAlgId::PUSH_IN || cr_alg_ID == CrAlgId::LOCAL_ASSEMBLY) {
            auto pad = reader.read(8);
            auto buffer = reader.read(24);
            crps_info = util::make_unique<CrpsInfo>(pad, buffer);
        }
    }

    virtual ~ParameterSetCrps() = default;

    void setCrpsInfo(std::unique_ptr<CrpsInfo> _crps_info);

    const CrpsInfo &getCrpsInfo() const { return *crps_info; }

    virtual void write(util::BitWriter &bw) const;

    std::unique_ptr<ParameterSetCrps> clone() const {
        auto ret = util::make_unique<ParameterSetCrps>(cr_alg_ID);
        ret->setCrpsInfo(crps_info->clone());
        return ret;
    }
};
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PARAMETER_SET_CRPS_H
