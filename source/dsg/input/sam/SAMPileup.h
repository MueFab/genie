/** @file SAMPileup.h
 *  @brief This file contains the definition of the SAMPileup class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_IO_SAM_SAMPILEUP_H_
#define CALQ_IO_SAM_SAMPILEUP_H_

#include <string>
#include <vector>

#include "Common/constants.h"

namespace calq {

class SAMPileup {
 public:
    SAMPileup(void);
    ~SAMPileup(void);

    bool empty(void) const;
    void clear(void);
    void print(void) const;
    void printQual(void) const;
    void printSeq(void) const;

    uint32_t pos;  // 0-based position of this pileup
    std::string qual;
    std::string seq;

    char ref;

    uint16_t hq_softcounter;  // High quality softclips next to this position
};

}  // namespace calq

#endif  // CALQ_IO_SAM_SAMPILEUP_H_

