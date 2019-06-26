/** @file SAMRecord.h
 *  @brief This file contains the definition of the SAMRecord class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_IO_SAM_SAMRECORD_H_
#define CALQ_IO_SAM_SAMRECORD_H_

#include <inttypes.h>

#include <deque>
#include <string>

#include "IO/SAM/SAMPileupDeque.h"

namespace calq {

class SAMRecord {
 public:
    static const int NUM_FIELDS = 12;
    
    SAMRecord(char* fields[]);
    explicit SAMRecord(const uint32_t pos, const std::string& cigar, const std::string& seq, const std::string& qual);
    ~SAMRecord(void);

    void addToPileupQueue(SAMPileupDeque *samPileupDeque) const;

    bool isMapped(void) const;
    void printLong(void) const;
    void printShort(void) const;
    void printSeqWithPositionOffset(void) const;

    std::string qname;  // Query template NAME
    uint16_t    flag;   // bitwise FLAG (uint16_t)
    std::string rname;  // Reference sequence NAME
    uint32_t    pos;    // 1-based leftmost mapping POSition (uint32_t)
    uint8_t     mapq;   // MAPping Quality (uint8_t)
    std::string cigar;  // CIGAR string
    std::string rnext;  // Ref. name of the mate/NEXT read
    uint32_t    pnext;  // Position of the mate/NEXT read (uint32_t)
    int64_t     tlen;   // observed Template LENgth (int64_t)
    std::string seq;    // segment SEQuence
    std::string qual;   // QUALity scores
    std::string opt;    // OPTional information

    uint32_t posMin;  // 0-based leftmost mapping position
    uint32_t posMax;  // 0-based rightmost mapping position

 private:
    void computeMappingPositions(void);
    void check(void);

 private:
    bool mapped_;
};

}  // namespace calq

#endif  // CALQ_IO_SAM_SAMRECORD_H_
