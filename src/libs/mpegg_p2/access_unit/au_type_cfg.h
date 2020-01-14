#ifndef GENIE_AU_TYPE_CFG_H
#define GENIE_AU_TYPE_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

#include "extended_au.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {

/**
 * ISO 23092-2 Section 3.4.1.1 table 19 lines 16 to 25
 */
class AuTypeCfg {
   private:
    uint16_t sequence_ID;                     //!< Line 18
    uint64_t AU_start_position;               //!< Line 19
    uint64_t AU_end_position;                 //!< Line 20
    std::unique_ptr<ExtendedAu> extended_AU;  //!< Lines 21 to 24

    uint8_t posSize;  //!< internal

   public:
    AuTypeCfg(uint16_t _sequence_ID, uint64_t _AU_start_position, uint64_t _AU_end_position, uint8_t posSize);
    explicit AuTypeCfg(uint8_t posSize);
    AuTypeCfg(uint8_t _posSize, bool multiple_alignments, util::BitReader &reader);
    virtual ~AuTypeCfg() = default;

    void setExtendedAu(std::unique_ptr<ExtendedAu> _extended_AU);

    uint16_t getRefID() const;

    uint64_t getStartPos() const;

    uint64_t getEndPos() const;

    virtual void write(util::BitWriter &writer);
};
}  // namespace mpegg_p2
}  // namespace genie

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_AU_TYPE_CFG_H
