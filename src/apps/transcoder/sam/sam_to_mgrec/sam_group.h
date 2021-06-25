/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SAM_GROUP_H_
#define SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SAM_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <map>
#include <tuple>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

#include "apps/transcoder/sam/sam_to_mgrec/sam_record.h"
#include "genie/core/record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

/**
 * @brief Helping structure to sort the records
 */
class SamRecordGroup {
 public:
    /**
     * @brief
     */

    enum class TemplateType : uint8_t {
        SINGLE = 0,
        PAIRED_1 = 1,
        PAIRED_2 = 2,
        PAIRED_UNKNOWN = 3,
        TOTAL_INDICES = 4,  // Not used
    };

    enum class MappingType : uint8_t {
        UNMAPPED = 0,
        PRIMARY = 1,
        NONPRIMARY = 2,
        TOTAL_INDICES = 3,  // Not used
    };



 private:

    std::vector<std::vector<std::vector<SamRecord>>> data;  //!< @brief

    /**
     * @brief
     * @param _r1
     * @param r2
     * @param force_split
     * @return
     */
    genie::core::record::Record pairedEndedToMpegg(SamRecord& _r1, SamRecord* r2, bool force_split = true);

    /**
     * @brief
     * @param rec
     * @param r
     * @param other_r
     * @param force_split
     */
    void addAlignment(genie::core::record::Record &rec, SamRecord *r1, SamRecord *r2, bool paired_end, bool force_split = false);

    /**
     * @brief
     * @param records
     * @param sam_rec
     */
    void convertUnmapped(std::list<genie::core::record::Record>& records, SamRecord& sam_rec);

    /**
     * @brief
     * @param records
     * @param sam_recs
     * @param unmapped_pair
     * @param is_read_1_first
     */
    void convertSingleEnd(std::list<genie::core::record::Record>& records, std::list<SamRecord>& sam_recs,
                          bool unmapped_pair = false, bool is_read_1_first = true);

    /**
     * @brief
     * @param mgrecs_by_rid
     * @param recs_rid_order
     * @param rec
     * @param other_rec
     */
    void createMgrecAndAddAlignment(std::map<int32_t, genie::core::record::Record>& mgrecs_by_rid,
                                    std::vector<int32_t>& recs_rid_order, SamRecord& rec, SamRecord* other_rec);

    /**
     * @brief
     * @param recs1_by_rid
     * @param recs1_rid_order
     * @param recs2_by_rid
     * @param recs2_rid_order
     * @param sam_recs_2d
     */
    void convertPairedEnd(std::map<int32_t, genie::core::record::Record>& recs1_by_rid,
                          std::vector<int32_t>& recs1_rid_order,
                          std::map<int32_t, genie::core::record::Record>& recs2_by_rid,
                          std::vector<int32_t>& recs2_rid_order, std::list<std::list<SamRecord>> sam_recs_2d);

    /**
     * @brief
     * @param recs_by_rid
     * @param recs_rid_order
     */
    void handlesMoreAlignments(std::map<int32_t, genie::core::record::Record>& recs_by_rid,
                               std::vector<int32_t>& recs_rid_order);

 public:
    /**
     * @brief
     */
    SamRecordGroup();

    /**
     * @brief
     * @param flags
     * @return
     */
    static std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);


    /**
     *
     * @return
     */
    std::pair<bool, genie::core::record::ClassType> validate();

    /**
     * @brief
     * @param rec
     */
    void addRecord(SamRecord&& rec);

    /**
     * @brief
     * @param records
     * @param create_same_rec
     */
    void convert(std::list<genie::core::record::Record>& records, bool create_same_rec = false);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcoder
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SAM_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
