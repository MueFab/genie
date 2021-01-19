/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE
#define GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE

#include <string>
#include <vector>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exception.h>
#include <genie/format/mpegg_p1/util.h>

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
    * Reference - ISO 23092-1 Section 6.5.1.3 table 10
    * Reference Metadata - ISO 23092-1 Section 6.5.1.4 table 13 - metadata specified in ISO/IEC 23092-3
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
//    std::unique_ptr<ReferenceLocation> reference_location;

   public:

    /**
     *
     */
    Reference();

    /**
     *
     * @param _ds_group_ID
     * @param _ref_ID
     * @param _ref_name
     * @param _ref_major_ver
     * @param _ref_minor_ver
     * @param _ref_patch_ver
     * @param _seq_names
     * @param _ref_loc
     */
    Reference(uint8_t _ds_group_ID, uint8_t _ref_ID, std::string _ref_name, uint16_t _ref_major_ver,
              uint16_t _ref_minor_ver, uint16_t _ref_patch_ver, std::vector<std::string>&& _seq_names,
              ReferenceLocation&& _ref_loc);

    /**
     *
     * @param reader
     * @param length
     */
    Reference(util::BitReader& reader, size_t length);

    /**
     *
     * @param _dataset_group_ID
     */
    void setDatasetGroupId(uint8_t _dataset_group_ID);

    /**
     *
     * @return
     */
    uint8_t getDatasetGroupId() const;

    /**
     *
     * @param _seq_names
     */
    void addSequenceNames(std::vector<std::string>&& _seq_names);

    /**
     *
     * @return
     */
    const std::vector<std::string>& getSequenceNames() const;

    /**
     *
     * @param _ref_loc
     */
     //TODO: FIX ME
//    void addReferenceLocation(ReferenceLocation&& _ref_loc);

    /**
     *
     * @return
     */
    //TODO: FIX ME
//    const ReferenceLocation& getReferenceLocation() const;

    /**
     *
     * @return
     */
    uint16_t getSeqCount() const;

    /**
     * Get length of reference in bytes
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE
