
#ifndef GENIE_PART1_REFERENCE_H
#define GENIE_PART1_REFERENCE_H

#include <string>
#include <vector>

#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "reference_location/reference_location.h"
#include "reference_location/external.h"
#include "reference_location/internal.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class Reference{
   public:
    enum class ReferenceType : uint8_t {
        MPEGG_REF = 0,
        RAW_REF = 1,
        FASTA_REF = 2
    };

   private:
    /** ------------------------------------------------------------------------------------------------------------
    * ISO 23092-1 Section 6.5.1.3 table 10
    * ISO 23092-1 Section 6.5.1.4 table 13 - metadata specified in ISO/IEC 23092-3
    * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID;
    uint8_t reference_ID;
    std::string reference_name;
    uint16_t reference_major_version;
    uint16_t reference_minor_version;
    uint16_t reference_patch_version;

    // seq_count is length of vector
    std::vector<std::string> sequence_names;

    // Contains external_ref_flag and corresponding data
    ReferenceLocation reference_location;

   public:

    Reference();
    Reference(uint8_t _dataset_grp_ID, uint8_t _ref_ID, std::string _ref_name, uint16_t _ref_major_ver,
              uint16_t _ref_minor_ver, uint16_t _ref_patch_ver);

    void setSequenceNames(std::vector<std::string>&& _seq_names);

    void addReferenceLocation(ReferenceLocation&& _ref_loc);

    uint16_t getSeqCount() const;

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_REFERENCE_H
