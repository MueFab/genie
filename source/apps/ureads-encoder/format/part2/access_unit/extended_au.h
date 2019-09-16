#ifndef GENIE_EXTENDED_AU_H
#define GENIE_EXTENDED_AU_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------------------------------------------

namespace format {

class BitWriter;

/**
 * ISO 23092-2 Section 3.4.1.1 table 19 lines 21 to 24
 */
class ExtendedAu {
   private:
    uint64_t extended_AU_start_position;  //!< Line 22
    uint64_t extended_AU_end_position;    //!< Line 23

    uint8_t posSize;  //!< Internal
   public:
    ExtendedAu(uint64_t _extended_AU_start_position, uint64_t _extended_AU_end_position, uint8_t _posSize);

    virtual void write(BitWriter *writer);
};
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXTENDED_AU_H