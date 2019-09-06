#include <ureads-encoder/exceptions.h>
#include "parameter_set_crps.h"
#include "ureads-encoder/format/part2/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    ParameterSetCrps::ParameterSetCrps(CrAlgId _cr_alg_ID) : cr_alg_ID(_cr_alg_ID){
        if(cr_alg_ID == CrAlgId::PUSH_IN || cr_alg_ID == CrAlgId::LOCAL_ASSEMBLY) {
            crps_info = make_unique<CrpsInfo>();
        }
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSetCrps::setCrpsInfo(std::unique_ptr<CrpsInfo> _crps_info) {
        if(!crps_info){
            GENIE_THROW_RUNTIME_EXCEPTION("Invalid crps mode for crps info");
        }
        crps_info = std::move(_crps_info);
    }
};