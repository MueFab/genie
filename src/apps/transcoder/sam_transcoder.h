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
        SINGLE_UNMAPPED = 0,
        SINGLE_MAPPED = 1,
        PAIR_FIRST = 2,
        PAIR_LAST = 3,
        UNKNOWN = 4,
        TOTAL_TYPES = 5,  // Not used
    };

   private:
    std::vector<std::list<SamRecord>> data;

    std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    void convertUnmapped(std::list<genie::core::record::Record> &records);
    void convertSingleEnd(std::list<genie::core::record::Record> &records, bool is_read_1_first=true);
    void convertPairedEnd(std::list<genie::core::record::Record> &records, bool force_split);

   public:
    SamRecordGroup();
    void addRecord(SamRecord &&rec);
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
    bool isSingle();

    /**
     * @brief Check if sam records in ReadTemplate are paired-end reads
     *
     * @return
     */
    bool isPair();

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
