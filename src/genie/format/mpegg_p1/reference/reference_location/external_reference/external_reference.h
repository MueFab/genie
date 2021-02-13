#ifndef GENIE_PART1_EXTERNAL_REFERENCE_H
#define GENIE_PART1_EXTERNAL_REFERENCE_H

#include <cstdint>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exception.h>
#include <genie/util/runtime-exception.h>

#include "checksum.h"

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

    ExternalReference();

    virtual ~ExternalReference() = default;

//    explicit ExternalReference(util::BitReader& reader);

    explicit ExternalReference(Type _reference_type);

    virtual Checksum::Algo getChecksumAlg() const = 0;

    Type getReferenceType() const;

    virtual uint64_t getLength() = 0;

    virtual void write(genie::util::BitWriter& writer) = 0;

   private:
    Type reference_type;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_H
