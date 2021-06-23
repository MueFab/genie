/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_EXTERNAL_REFERENCE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_EXTERNAL_REFERENCE_H_

#include <cstdint>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ExternalReference {
 public:
    enum class Type : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };

    ExternalReference();

    //    explicit ExternalReference(util::BitReader& reader);

    explicit ExternalReference(Type _reference_type);

    ~ExternalReference() = default;

    virtual Checksum::Algo getChecksumAlg() const;

    Type getReferenceType() const;

    virtual uint64_t getLength();

    virtual void write(genie::util::BitWriter& writer);

 private:
    Type reference_type;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_LOCATION_EXTERNAL_REFERENCE_EXTERNAL_REFERENCE_H_
