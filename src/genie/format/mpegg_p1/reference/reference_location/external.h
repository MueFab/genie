#ifndef GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H
#define GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H

#include <string>
#include <memory>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

#include "external_reference/external_reference.h"
#include "reference_location.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class External : public ReferenceLocation {
   private:
    std::string ref_uri;
    std::unique_ptr<ExternalReference> external_reference;

   public:

//    External(std::string&& _ref_uri, ExternalReference&& _ext_ref);

    explicit External(util::BitReader& reader, uint16_t seq_count);

    std::string getRefUri() const;

    Checksum::Algo getChecksumAlg() const;

    uint64_t getLength() const;

    void write(genie::util::BitWriter& writer) const override;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_REFERENCE_LOCATION_EXTERNAL_H
