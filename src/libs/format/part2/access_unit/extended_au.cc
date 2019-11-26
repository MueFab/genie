#include "extended_au.h"

#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
void ExtendedAu::write(util::BitWriter *writer) {
    writer->write(extended_AU_start_position, posSize);
    writer->write(extended_AU_end_position, posSize);
}

// -----------------------------------------------------------------------------------------------------------------

ExtendedAu::ExtendedAu(uint64_t _extended_AU_start_position, uint64_t _extended_AU_end_position, uint8_t _posSize)
    : extended_AU_start_position(_extended_AU_start_position),
      extended_AU_end_position(_extended_AU_end_position),
      posSize(_posSize) {}
}  // namespace format
