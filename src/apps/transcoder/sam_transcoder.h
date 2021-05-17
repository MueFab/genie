#ifndef SAM_TRANSCODER_H
#define SAM_TRANSCODER_H

#include <htslib/sam.h>
#include <vector>
#include <list>
#include <genie/core/record/record.h>

#include "sam_record.h"

namespace sam_transcoder {

class SamRecordGroup {  // Helping structure to sort the records
   public:
    enum class Index : uint8_t {
        UNMAPPED = 0,
        NOT_PAIRED = 1,
        PAIR_READ1 = 2,
        PAIR_READ2 = 3,
        UNKNOWN = 4,
        TOTAL_TYPES = 5,  // Not used
    };

   private:
    std::vector<std::list<Record>> data;

    std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    void convertUnmapped(std::list<genie::core::record::Record> &records);
    void convertSingleEnd(std::list<genie::core::record::Record> &records,
                          bool unmapped_pair=false, bool is_read_1_first=true);
    void convertPairedEnd(std::list<genie::core::record::Record> &records,
                          bool force_split=false);

   public:
    SamRecordGroup();

    bool getRecords(std::list<std::list<Record>>& sam_recs);

    void addRecord(Record &&rec);

    /**
     * @brief Check if sam records in ReadTemplate are unmapped
     *
     * @return
     */
    bool isCatUnmapped();

    /**
     * @brief Check if sam records in ReadTemplate are single-end reads
     *
     * @return
     */
    bool isCatNotPaired();

    /**
     * @brief Check if sam records in ReadTemplate are paired-end reads
     *
     * @return
     */
    bool isCatPaired();

    /**
     * @brief Check if sam records in ReadTemplate cannot be categorized
     *
     * @return
     */
    bool isCatUnknown();

    /**
     * @brief Check if sam records are valid and does not belongs to 2 or more different category
     *
     * @return
     */
    bool isValid();

    void convert(std::list<genie::core::record::Record> &records, bool force_split=false);

    /**
     *
     * @param token
     * @return
     */
    static char convertCigar2ECigarChar(char token);

    /**
     *
     * @param token
     * @return
     */
    static int stepSequence(char token);

    /**
     * Convert CIGAR string to ECIGAR string
     *
     * @param cigar
     * @param seq
     * @return
     */
    static std::string convertCigar2ECigar(const std::string &cigar, const std::string &seq);
};

uint8_t sam_to_mgrec(transcoder::ProgramOptions& options);

}

#endif  // SAM_TRANSCODER_H
