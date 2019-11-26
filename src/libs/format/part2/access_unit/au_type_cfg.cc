#include "au_type_cfg.h"

#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
void AuTypeCfg::write(util::BitWriter *writer) {
    writer->write(sequence_ID, 16);
    writer->write(AU_start_position, posSize);
    writer->write(AU_end_position, posSize);
    if (extended_AU) {
        extended_AU->write(writer);
    }
}

// -----------------------------------------------------------------------------------------------------------------

AuTypeCfg::AuTypeCfg(uint16_t _sequence_ID, uint64_t _AU_start_position, uint64_t _AU_end_position, uint8_t _posSize)
    : sequence_ID(_sequence_ID),
      AU_start_position(_AU_start_position),
      AU_end_position(_AU_end_position),
      posSize(_posSize) {}

// -----------------------------------------------------------------------------------------------------------------

AuTypeCfg::AuTypeCfg(uint8_t _posSize) : AuTypeCfg(0, 0, 0, _posSize) {}

// -----------------------------------------------------------------------------------------------------------------

void AuTypeCfg::setExtendedAu(std::unique_ptr<ExtendedAu> _extended_AU) { extended_AU = std::move(_extended_AU); }
}  // namespace format
