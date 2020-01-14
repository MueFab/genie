#include "extended_au.h"

#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
void ExtendedAu::write(util::BitWriter& writer) {
    writer.write(extended_AU_start_position, posSize);
    writer.write(extended_AU_end_position, posSize);
}

// -----------------------------------------------------------------------------------------------------------------

ExtendedAu::ExtendedAu(uint64_t _extended_AU_start_position, uint64_t _extended_AU_end_position, uint8_t _posSize)
    : extended_AU_start_position(_extended_AU_start_position),
      extended_AU_end_position(_extended_AU_end_position),
      posSize(_posSize) {}

ExtendedAu::ExtendedAu(uint8_t _posSize, util::BitReader& reader) : posSize(_posSize) {
    extended_AU_start_position = reader.read(posSize);
    extended_AU_end_position = reader.read(posSize);
}
}  // namespace mpegg_p2
}  // namespace genie