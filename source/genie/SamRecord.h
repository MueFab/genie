// Copyright 2018 The genie authors


/**
 *  @file SamRecord.h
 *  @brief SAM record interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_INPUT_SAM_SAMRECORD_H_
#define DSG_INPUT_SAM_SAMRECORD_H_


#include <inttypes.h>

#include <string>
#include <vector>

#include "genie/SamRecord.h"


namespace dsg {
namespace input {
namespace sam {


struct SamRecord {
 public:
    static const int NUM_FIELDS = 12;

    SamRecord(
        const std::vector<std::string>& fields);

    ~SamRecord(void);

 public:
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
};


}  // namespace sam
}  // namespace input
}  // namespace dsg


#endif  // DSG_INPUT_SAM_SAMRECORD_H_
