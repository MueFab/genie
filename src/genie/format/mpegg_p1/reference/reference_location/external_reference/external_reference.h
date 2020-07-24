//
// Created by sxperfect on 10.07.20.
//

#ifndef GENIE_PART1_EXTERNAL_REFERENCE_H
#define GENIE_PART1_EXTERNAL_REFERENCE_H

#include <cstdint>

#include "checksum.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ExternalReference{
   public:
    enum class Type: uint8_t {
        MPEGG_REF = 0,
        RAW_REF = 1,
        FASTA_REF = 2
    };

    explicit ExternalReference(Type _reference_type);

    ~ExternalReference() = default;

    virtual void write(genie::util::BitWriter& bit_writer) const;

   protected:
    Type reference_type;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_H
