/** @file SAMRecord.cc
 *  @brief This file contains the implementation of the SAMRecord class.
 */

// Copyright 2015-2018 Leibniz Universitaet Hannover

#include "IO/SAM/SAMRecord.h"

#include <string.h>
#include <queue>
#include <limits>

#include "Common/Exceptions.h"
#include "Common/log.h"
#include "IO/FASTA/FASTAFile.h"

// If 1: consider softclips only at the end of the read. Id 0: consider softclips everywhere
#define BORDERSTRATEGY 0

namespace calq {

SAMRecord::SAMRecord(char *fields[NUM_FIELDS])
    : qname(fields[0]),
      flag((uint16_t)atoi(fields[1])),
      rname(fields[2]),
      pos((uint32_t)atoi(fields[3])),
      mapq((uint8_t)atoi(fields[4])),
      cigar(fields[5]),
      rnext(fields[6]),
      pnext((uint32_t)atoi(fields[7])),
      tlen((int64_t)atoi(fields[8])),
      seq(fields[9]),
      qual(fields[10]),
      opt(fields[11]),
      posMin(0),
      posMax(0),
      mapped_(false) {
    check();

    if (mapped_ == true) {
        // Compute 0-based first position and 0-based last position this record
        // is mapped to on the reference used for alignment
        posMin = pos - 1;
        posMax = pos - 1;

        size_t cigarIdx = 0;
        size_t cigarLen = cigar.length();
        uint32_t opLen = 0;  // length of current CIGAR operation

        for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
            if (isdigit(cigar[cigarIdx])) {
                opLen = opLen * 10 + (uint32_t)cigar[cigarIdx] - (uint32_t)'0';
                continue;
            }
            switch (cigar[cigarIdx]) {
            case 'M':
            case '=':
            case 'X':
                posMax += opLen;
                break;
            case 'I':
            case 'S':
                break;
            case 'D':
            case 'N':
                posMax += opLen;
                break;
            case 'H':
            case 'P':
                break;  // these have been clipped
            default:
                throwErrorException("Bad CIGAR string");
            }
            opLen = 0;
        }
        posMax -= 1;
    }
}

SAMRecord::~SAMRecord(void) {}

void SAMRecord::addToPileupQueue(SAMPileupDeque *samPileupDeque_, const FASTAFile& f, size_t qualityOffset) const {
    if (samPileupDeque_->empty() == true) {
        throwErrorException("samPileupQueue is empty");
    }
    if ((samPileupDeque_->posMin() > posMin) || (samPileupDeque_->posMax() < posMax)) {
        throwErrorException("samPileupQueue does not overlap record");
    }

    size_t cigarIdx = 0;
    size_t cigarLen = cigar.length();
    size_t opLen = 0;  // length of current CIGAR operation
    size_t idx = 0;
    size_t pileupIdx = posMin - samPileupDeque_->posMin();

    size_t softclips = 0;

    for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
        if (isdigit(cigar[cigarIdx])) {
            opLen = opLen*10 + (size_t)cigar[cigarIdx] - (size_t)'0';
            continue;
        }

        const char HQ_SOFTCLIP_THRESHOLD = 29 + qualityOffset;


        switch (cigar[cigarIdx]) {
        case 'M':
        case '=':
        case 'X':
            for (size_t i = 0; i < opLen; i++) {
                samPileupDeque_->pileups_[pileupIdx].pos = samPileupDeque_->posMin() + pileupIdx;
                samPileupDeque_->pileups_[pileupIdx].seq += seq[idx];
                samPileupDeque_->pileups_[pileupIdx].qual += qual[idx];
                samPileupDeque_->pileups_[pileupIdx].ref = f.references.at(rname)[pileupIdx];

                idx++; pileupIdx++;
            }
            break;
        case 'S':
            for (int l = 0; l < static_cast<int>(opLen); ++l) {
                if (this->qual[idx+l] >= HQ_SOFTCLIP_THRESHOLD) {
                    ++softclips;
                }
            }
        case 'I':
            idx += opLen;
            break;
        case 'D':
        case 'N':
            pileupIdx += opLen;
            break;
        case 'H':
        case 'P':
            break;  // these have been clipped
        default:
            throwErrorException("Bad CIGAR string");
        }

        opLen = 0;
    }

    // Write clips
    for (size_t i = posMin - samPileupDeque_->posMin(); i <= pileupIdx; ++i) {
        samPileupDeque_->pileups_[pileupIdx].hq_softcounter += softclips;
    }
}

bool SAMRecord::isMapped(void) const {
    return mapped_;
}

void SAMRecord::printLong(void) const {
    printShort();
    printf("isMapped: %d, ", mapped_);
    printf("posMin: %d, ", posMin);
    printf("posMax: %d\n", posMax);
}

void SAMRecord::printShort(void) const {
    printf("%s\t", qname.c_str());
    printf("%d\t", flag);
    printf("%s\t", rname.c_str());
    printf("%d\t", pos);
    printf("%d\t", mapq);
    printf("%s\t", cigar.c_str());
    printf("%s\t", rnext.c_str());
    printf("%d\t", pnext);
    printf("%" PRId64 "\t", tlen);
    printf("%s\t", seq.c_str());
    printf("%s\t", qual.c_str());
    printf("%s\t", opt.c_str());
    printf("\n");
}

void SAMRecord::printSeqWithPositionOffset(void) const {
    printf("%s %6d-%6d|", rname.c_str(), posMin, posMax);
    for (unsigned int i = 0; i < posMin; i++) { printf(" "); }
    printf("%s\n", seq.c_str());
}

void SAMRecord::check(void) {
    // Check all fields
    if (qname.empty() == true) { throwErrorException("qname is empty"); }
    // flag
    if (rname.empty() == true) { throwErrorException("rname is empty"); }
    // pos
    // mapq
    if (cigar.empty() == true) { throwErrorException("cigar is empty"); }
    if (rnext.empty() == true) { throwErrorException("rnext is empty"); }
    // pnext
    // tlen
    if (seq.empty() == true) { throwErrorException("seq is empty"); }
    if (qual.empty() == true) { throwErrorException("qual is empty"); }
    if (opt.empty() == true) { CALQ_LOG("opt is empty"); }

    // Check if this record is mapped
    if ((flag & 0x4) != 0) {
        mapped_ = false;
    } else {
        mapped_ = true;
        if (rname == "*" || pos == 0 || cigar == "*" || seq == "*" || qual == "*") {
            throwErrorException("Corrupted record");
        }
    }
}

}  // namespace calq

