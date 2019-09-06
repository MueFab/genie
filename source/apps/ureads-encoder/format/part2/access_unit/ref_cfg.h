#ifndef GENIE_REF_CFG_H
#define GENIE_REF_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    class BitWriter;

    /**
    * ISO 23092-2 Section 3.4.1.1 table 19 lines 11 to 15
    */
    class RefCfg {
    private:
        uint16_t ref_sequence_ID : 16; //!< Line 12
        uint64_t ref_start_position; //!< Line 13
        uint64_t ref_end_position;  //!< Line 14

        uint8_t posSize; //!< internal
    public:
        RefCfg(uint16_t _ref_sequence_ID, uint64_t _ref_start_position, uint64_t _ref_end_position, uint8_t _posSize);

        virtual void write(BitWriter *writer);
    };
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_REF_CFG_H
