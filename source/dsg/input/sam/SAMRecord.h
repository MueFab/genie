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
#include "Common/constants.h"

namespace calq {

class FASTAFile;

class SAMRecord {
 public:
    static const int NUM_FIELDS = 12;

    explicit SAMRecord(char *fields[NUM_FIELDS]);
    ~SAMRecord(void);

    void addToPileupQueue(SAMPileupDeque *samPileupDeque, const FASTAFile& f, size_t qualityOffset) const;

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
    void check(void);
    bool mapped_;
};

}  // namespace calq

#endif  // CALQ_IO_SAM_SAMRECORD_H_

