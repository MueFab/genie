#ifndef SAM_RECORD_H
#define SAM_RECORD_H

#include <htslib/sam.h>
#include <vector>
//#include <list>
//#include <genie/core/record/record.h>

#include "program-options.h"

namespace sam_transcoder {

class SamRecord {
  private:
    std::string qname; // Query template name
    uint16_t flag; // Flag
    int32_t rid; // Reference sequence ID
    uint32_t pos; // Position
    uint8_t mapq; // Mapping Quality
    std::string cigar; // CIGAR
    int32_t mate_rid; // Mate reference sequence ID
    uint32_t mate_pos; // Mate position
    int64_t tlen; // Observed template length
    std::string seq; // Read sequence
    std::string qual;
  public:
    static char fourBitBase2Char(uint8_t int_base);

    static std::string getCigarString(bam1_t *sam_alignment);

    static std::string getSeqString(bam1_t *sam_alignment);

    static std::string getQualString(bam1_t *sam_alignment);

    static char convertCigar2ECigarChar(char token);

    static int stepSequence(char token);

    static std::string convertCigar2ECigar(const std::string &cigar, const std::string &seq);

    explicit SamRecord(bam1_t *sam_alignment);

    const std::string& getQname();

    std::string&& moveQname();

    uint16_t getFlag() const;

    int32_t getRID() const;

    uint32_t getPos() const;

    uint8_t getMapq() const;

    const std::string& getCigar() const;

    std::string&& moveCigar();

    std::string getECigar() const;

    int32_t getMRID() const;

    uint32_t getMPos() const;

    const std::string& getSeq() const;

    std::string &&moveSeq();

    const std::string& getQual() const;

    std::string&& moveQual();

    bool checkFlag(uint16_t _flag) const;

    bool checkNFlag(uint16_t _flag) const;

    bool isUnmapped() const;

    bool isPrimary() const;

    bool isSecondary() const;

    bool isDuplicates() const;

    bool isSupplementary() const;

    bool isPaired() const;

    bool isRead1() const;

    bool isRead2() const;

    bool isProperlyPaired() const;

    bool isPairedAndBothMapped() const;

    bool isReverse() const;
};


}

#endif  // SAM_RECORD_H
