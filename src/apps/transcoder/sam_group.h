#ifndef TRANSCODER_SAM_GROUP_H
#define TRANSCODER_SAM_GROUP_H

#include <tuple>
#include <vector>
#include <list>

#include <genie/core/record/record.h>
#include <record/alignment_split/unpaired.h>

#include "sam_record.h"

namespace sam_transcoder {

class SamRecordGroup {  // Helping structure to sort the records
   public:
    enum class Index : uint8_t {
        UNMAPPED = 0,
        SINGLE = 1,
        PAIR_READ1_PRIMARY = 2,
        PAIR_READ2_PRIMARY = 3,
        PAIR_READ1_NONPRIMARY = 4,
        PAIR_READ2_NONPRIMARY = 5,
        UNKNOWN = 6,
        TOTAL_INDICES = 7,  // Not used
    };

    enum class Category : uint8_t {
        INVALID = 0,
        UNMAPPED = 1,
        SINGLE = 2,
        PAIRED = 4,
    };

   private:
    std::vector<std::list<SamRecord>> data;


    void convertUnmapped(std::list<genie::core::record::Record>& records,
                         SamRecord& sam_rec);

    void convertSingleEnd(std::list<genie::core::record::Record> &records,
                          std::list<SamRecord>& sam_recs,
                          bool unmapped_pair=false,
                          bool is_read_1_first=true);

    void convertPairedEnd(std::list<genie::core::record::Record> &records,
                          std::list<std::list<SamRecord>>& sam_recs_2d,
                          bool force_split=false);

   public:
    SamRecordGroup();

    static std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    SamRecordGroup::Category getRecords(std::list<std::list<SamRecord>>& sam_recs);

    void addRecord(SamRecord&&rec);

    /**
     * @brief Check if sam records in ReadTemplate are unmapped
     *
     * @return
     */
    bool isUnmapped();

    /**
     * @brief Check if sam records in ReadTemplate are single-end reads
     *
     * @return
     */
    bool isSingle() const;

    /**
     * @brief Check if sam records in ReadTemplate are paired-end reads
     *
     * @return
     */
    bool isPaired() const;

    /**
     * @brief Check if sam records in ReadTemplate cannot be categorized
     *
     * @return
     */
    bool isUnknown();

    /**
     * @brief Check if sam records are valid and does not belongs to 2 or more different category
     *
     * @return
     */
    bool isValid();

    Category computeClass();

    void convert(std::list<genie::core::record::Record> &records,
                 bool force_split=false);
};

}

#endif  // TRANSCODER_SAM_GROUP_H
