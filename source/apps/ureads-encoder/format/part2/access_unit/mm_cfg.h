#ifndef GENIE_MM_CFG_H
#define GENIE_MM_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    class BitWriter;

    /**
    * ISO 23092-2 Section 3.4.1.1 table 19 lines 7 to 10
    */
    class MmCfg {
    private:
        uint16_t mm_threshold : 16; //!< Line 8
        uint32_t mm_count : 32; //!< Line 9

    public:
        MmCfg(uint16_t _mm_threshold, uint32_t _mm_count);
        MmCfg();

        virtual void write(BitWriter *writer) const;
    };
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_MM_CFG_H
