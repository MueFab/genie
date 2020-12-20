#ifndef GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H
#define GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H

#include <string>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exception.h>
#include <genie/util/runtime-exception.h>

#include "reference_location.h"
#include "external_reference/external_reference.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class External : public ReferenceLocation {
   private:
    std::string ref_uri;
    Checksum::Algo checksum_alg;
    ExternalReference::Type reference_type;

    ExternalReference external_reference;


   public:

    External(std::string&& _ref_uri, ExternalReference&& _ext_ref);

    explicit External(util::BitReader& reader, uint16_t seq_count);

    std::string getRefUri() const;

    const Checksum::Algo getChecksumAlg() const;

    uint64_t getBitLength() const;

    void write(genie::util::BitWriter& writer) override;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H
