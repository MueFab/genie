#ifndef GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H
#define GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H

#include <string>

#include "reference_location.h"
#include "external_reference/external_reference.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class External : public ReferenceLocation {
   public:
    enum class ExternalReferenceType : uint8_t {
        MPEGG_REF = 0,
        RAW_REF = 1,
        FASTA_REF = 2
    };
   private:
    std::string ref_uri;
    ExternalReference external_reference;

   public:

    External(std::string&& _ref_uri, ExternalReference&& _ext_ref);

    void write(genie::util::BitWriter& bit_writer) const override;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H
