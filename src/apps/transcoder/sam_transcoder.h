#ifndef SAM_TRANSCODER_H
#define SAM_TRANSCODER_H

#include <htslib/sam.h>
#include <vector>
#include <list>
#include <genie/core/record/record.h>
#include <genie/util/bitwriter.h>

#include "sam_record.h"

namespace sam_transcoder {

#define PHASE1_EXT ".phase1.mgrec"
#define PHASE2_EXT ".phase2.mgrec"
#define PHASE2_TMP_EXT ".phase2.tmp"
#define BUFFER_SIZE 500000

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

    enum class Class : uint8_t {
        INVALID = 0,
        UNMAPPED = 1,
        SINGLE = 2,
        PAIRED = 4,
    };

   private:
    std::vector<std::list<SamRecord>> data;

    std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    void convertUnmapped(std::list<genie::core::record::Record>& records, SamRecord& sam_rec);

    void convertSingleEnd(std::list<genie::core::record::Record> &records,
                          std::list<SamRecord>& sam_recs,
                          bool unmapped_pair=false,
                          bool is_read_1_first=true);

    void convertPairedEnd(std::list<genie::core::record::Record> &records,
                          std::list<std::list<SamRecord>>& sam_recs_2d,
                          bool force_split=false);

   public:
    SamRecordGroup();

    SamRecordGroup::Class getRecords(std::list<std::list<SamRecord>>& sam_recs);

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

    Class computeClass();

    void convert(std::list<genie::core::record::Record> &records, bool force_split=false);
};

bool save_mgrecs_by_rid(std::list<genie::core::record::Record>& mpegg_recs,
                        std::map<int32_t, genie::util::BitWriter>& bitwriters);

uint8_t sam_to_mgrec_phase1(transcoder::ProgramOptions& options, int& nref);

uint8_t sam_to_mgrec_phase2(transcoder::ProgramOptions& options, int& nref);

uint8_t sam_to_mgrec(transcoder::ProgramOptions& options);

}

#endif  // SAM_TRANSCODER_H
