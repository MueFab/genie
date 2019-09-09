#ifndef GENIE_PARAMETER_SET_CRPS_H
#define GENIE_PARAMETER_SET_CRPS_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>

#include "crps_info.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
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
        CrAlgId cr_alg_ID; //!<< : 8; Line 2
        std::unique_ptr<CrpsInfo> crps_info;  //!<< Lines 3 to 6

    public:
        explicit ParameterSetCrps(CrAlgId _cr_alg_ID);
        void setCrpsInfo(std::unique_ptr<CrpsInfo> _crps_info);
        virtual void write(BitWriter* bw) const;
    };
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_PARAMETER_SET_CRPS_H
