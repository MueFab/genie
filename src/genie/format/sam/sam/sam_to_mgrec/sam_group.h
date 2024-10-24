/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_SAM_TO_MGREC_SAM_GROUP_H_
#define SRC_GENIE_FORMAT_SAM_SAM_SAM_TO_MGREC_SAM_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <map>
#include <tuple>
#include <utility>
#include <vector>
#include "genie/format/sam/sam/sam_to_mgrec/sam_record.h"
#include "genie/core/record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam::sam::sam_to_mgrec {

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

    /**
     * @brief
     */
    enum class MappingType : uint8_t {
        UNMAPPED = 0,
        PRIMARY = 1,
        NONPRIMARY = 2,
        TOTAL_INDICES = 3,  // Not used
    };

 private:
    std::vector<std::vector<std::list<SamRecord>>> data;  //!< @brief

    /**
     * @brief
     * @param rec
     * @param r
     * @param other_r
     * @param force_split
     */
    static void addAlignment(genie::core::record::Record& rec, SamRecord* r1, SamRecord* r2, bool paired_end,
                             bool force_split = false);

    /**
     * @brief
     * @return
     */
    bool checkIfPaired();

    /**
     * @brief
     * @return
     */
    genie::core::record::ClassType checkClassTypeSingle();

    /**
     * @brief
     * @param tempType
     * @param mapType
     */
    void removeDuplicatesPaired(TemplateType tempType, MappingType mapType);

    /**
     * @brief
     * @param tempType
     * @return
     */
    size_t primaryTemplateCount(TemplateType tempType);

    /**
     * @brief
     * @param mapType
     * @return
     */
    size_t mappingCount(MappingType mapType);

    /**
     * @brief
     */
    void removeAmbiguousSecondaryAlignments();

    /**
     * @brief
     */
    void moveSecondaryAlignments();

    /**
     * @brief
     * @param unknown_count
     * @param read_1_count
     * @param read_2_count
     */
    void guessUnknownReadOrder(size_t& unknown_count, size_t& read_1_count, size_t& read_2_count);

    /**
     * @brief
     * @return
     */
    genie::core::record::ClassType checkClassTypePaired();

    /**
     * @brief
     * @return
     */
    std::pair<SamRecord*, SamRecord*> getReadTuple();

    /**
     * @brief
     * @param cls
     * @return
     */
    bool isR1First(const std::pair<bool, genie::core::record::ClassType>& cls);

    /**
     * @brief
     * @param rec
     * @param r1
     */
    static void addSegment(genie::core::record::Record& rec, SamRecord* r1);

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
     * @brief
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

}  // namespace genieapp::transcode_sam::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_SAM_TO_MGREC_SAM_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
