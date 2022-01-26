/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/reference/reference_location/external.h"
#include "genie/format/mpegg_p1/reference/reference_location/internal.h"
#include "genie/format/mpegg_p1/reference/reference_location/reference_location.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Reference {
 public:
    enum class ReferenceType : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };  //!< @brief

 private:
    /** ------------------------------------------------------------------------------------------------------------
     * Reference - ISO 23092-1 Section 6.5.1.3 table 10
     * Reference Metadata - ISO 23092-1 Section 6.5.1.4 table 13 - metadata specified in ISO/IEC 23092-3
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID;          //!< @brief
    uint8_t reference_ID;              //!< @brief
    std::string reference_name;        //!< @brief
    uint16_t reference_major_version;  //!< @brief
    uint16_t reference_minor_version;  //!< @brief
    uint16_t reference_patch_version;  //!< @brief

    std::vector<std::string> sequence_names;  //!< @brief seq_count is length of vector
    std::vector<uint32_t> sequence_lengths;   //!< @brief ISO 23092-1:2020 (2nd Ed.) Section 6.5.2.3.2
    std::vector<uint16_t> sequence_IDs;       //!< @brief ISO 23092-1:2020 (2nd Ed.) Section 6.5.2.3.2

    // Contains external_ref_flag and corresponding data
    std::unique_ptr<ReferenceLocation> reference_location;  //!< @brief

    std::string minor_version;  //!< @brief Additional

 public:
    /**
     * @brief
     */
    Reference();

    /**
     * @brief Read Reference from bitstream. FileHeader is required to determine syntax.
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    Reference(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     * @brief
     * @param _dataset_group_ID
     */
    void setDatasetGroupId(uint8_t _dataset_group_ID);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupId() const;

    /**
     * @brief
     * @param _seq_names
     */
    void addSequenceNames(std::vector<std::string>&& _seq_names);

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getSequenceNames() const;

    /**
     * @brief
     * @return
     */
    const ReferenceLocation& getReferenceLocation() const;

    /**
     * @brief
     * @return
     */
    uint16_t getSeqCount() const;

    /**
     * @brief Get length of reference in bytes
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
