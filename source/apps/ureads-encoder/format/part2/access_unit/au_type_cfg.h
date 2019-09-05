#ifndef GENIE_AU_TYPE_CFG_H
#define GENIE_AU_TYPE_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include "extended_au.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {

/**
* ISO 23092-2 Section 3.4.1.1 table 19 lines 16 to 25
*/
    class AuTypeCfg {
    private:
        uint16_t sequence_ID : 16; //!< Line 18
        uint64_t AU_start_position; // TODO: Size; //!< Line 19
        uint64_t AU_end_position; // TODO: Size; //!< Line 20
        std::unique_ptr <ExtendedAu> extended_AU; //!< Lines 21 to 24

    public:
        AuTypeCfg(uint16_t _sequence_ID, uint64_t _AU_start_position, uint64_t _AU_end_position)
                : sequence_ID(_sequence_ID), AU_start_position(_AU_start_position), AU_end_position(_AU_end_position) {

        }

        void setExtendedAu(std::unique_ptr <ExtendedAu> _extended_AU) {
            extended_AU = std::move(_extended_AU);
        }

        virtual void write(BitWriter *writer);
    };
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_AU_TYPE_CFG_H
