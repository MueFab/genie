/** @file QualEncoder.cc
 *  @brief This file contains the implementation of the QualEncoder class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "QualCodec/QualEncoder.h"

#include <math.h>

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <algorithm>

#include "Common/constants.h"
#include "Common/Exceptions.h"
#include "Common/log.h"
#include "QualCodec/Quantizers/UniformQuantizer.h"
#include "QualCodec/Quantizers/UniformMinMaxQuantizer.h"

#include "IO/FASTA/FASTAFile.h"

namespace calq {

QualEncoder::QualEncoder(const int &polyploidy,
                         const int &qualityValueMax,
                         const int &qualityValueMin,
                         const int &qualityValueOffset)
    : compressedMappedQualSize_(0),
      compressedUnmappedQualSize_(0),
      nrMappedRecords_(0),
      nrUnmappedRecords_(0),
      uncompressedMappedQualSize_(0),
      uncompressedUnmappedQualSize_(0),

      qualityValueOffset_(qualityValueOffset),
      posOffset_(0),

      unmappedQualityValues_(""),
      mappedQuantizerIndices_(),
      mappedQualityValueIndices_(),

      samPileupDeque_(),

      haplotyper_(17, polyploidy, qualityValueOffset, NR_QUANTIZERS, 50, 7, 50),
      posCounter(0),

      quantizers_(),

      samRecordDeque_() {
    if (polyploidy < 1) {
        throwErrorException("polyploidy must be greater than zero");
    }
    if (qualityValueMax < 0) {
        throwErrorException("qualityValueMax must be zero or greater");
    }
    if (qualityValueMin < 0) {
        throwErrorException("qualityValueMin must be zero or greater");
    }
    if (qualityValueOffset < 1) {
        throwErrorException("qualityValueOffset must be greater than zero");
    }

    // Construct quantizers
    int quantizerSteps = QUANTIZER_STEPS_MIN;
    int quantizerIdx = QUANTIZER_IDX_MIN;
    for (int i = 0; i < NR_QUANTIZERS; ++i) {
        Quantizer quantizer = UniformMinMaxQuantizer(qualityValueMin, qualityValueMax, quantizerSteps);
        quantizers_.insert(std::pair<int, Quantizer>(quantizerIdx, quantizer));
        quantizerSteps++;
        quantizerIdx++;
    }

    // Initialize a buffer for mapped quality value indices per quantizer
    for (int i = 0; i < NR_QUANTIZERS; ++i) {
        mappedQualityValueIndices_.push_back(std::deque<int>());
    }
}

QualEncoder::~QualEncoder(void) {}

void QualEncoder::addUnmappedRecordToBlock(const SAMRecord &samRecord) {
    encodeUnmappedQual(samRecord.qual);
    uncompressedUnmappedQualSize_ += samRecord.qual.length();
    nrUnmappedRecords_++;
}

void QualEncoder::addMappedRecordToBlock(const SAMRecord &samRecord, const FASTAFile& fasta) {
    if (nrMappedRecords() == 0) {
        posOffset_ = samRecord.posMin;
        samPileupDeque_.setPosMin(samRecord.posMin);
        samPileupDeque_.setPosMax(samRecord.posMax);
    }

    if (samRecord.posMax > samPileupDeque_.posMax()) {
        samPileupDeque_.setPosMax(samRecord.posMax);
    }

    samRecord.addToPileupQueue(&samPileupDeque_, fasta, qualityValueOffset_);
    samRecordDeque_.push_back(samRecord);

    while (samPileupDeque_.posMin() < samRecord.posMin) {
        int k = haplotyper_.push(samPileupDeque_.front().seq, samPileupDeque_.front().qual, samPileupDeque_.front().hq_softcounter,
                                 fasta.references.at(samRecord.rname)[samPileupDeque_.posMin()]);
        ++posCounter;
        // Start not until pipeline is full
        if (posCounter > haplotyper_.getOffset()) {
            mappedQuantizerIndices_.push_back(k);
        }
        samPileupDeque_.pop_front();
    }
    // Start not until pipeline is full
    while (samRecordDeque_.front().posMax+haplotyper_.getOffset() < samPileupDeque_.posMin()) {
        encodeMappedQual(samRecordDeque_.front());
        samRecordDeque_.pop_front();
    }

    uncompressedMappedQualSize_ += samRecord.qual.length();
    nrMappedRecords_++;
}

void QualEncoder::finishBlock(const FASTAFile& fasta, const std::string& section) {
    // Compute all remaining quantizers
    while (samPileupDeque_.empty() == false) {
        int k = haplotyper_.push(samPileupDeque_.front().seq, samPileupDeque_.front().qual, samPileupDeque_.front().hq_softcounter,
                                 fasta.references.at(section)[samPileupDeque_.posMin()]);
        ++posCounter;
        if (posCounter > haplotyper_.getOffset()) {
            mappedQuantizerIndices_.push_back(k);
        }
        samPileupDeque_.pop_front();
    }

    // Empty pipeline
    size_t offset = std::min(posCounter, haplotyper_.getOffset());
    for (size_t i = 0; i < offset; ++i) {
        int k = haplotyper_.push("", "", 0, 'N');
        mappedQuantizerIndices_.push_back(k);
    }

    // TODO(muenteferi): borders of blocks probably too low activity

    // Process all remaining records from queue
    while (samRecordDeque_.empty() == false) {
        encodeMappedQual(samRecordDeque_.front());
        samRecordDeque_.pop_front();
    }

    posCounter = 0;
}

size_t QualEncoder::writeBlock(CQFile *cqFile) {
    compressedMappedQualSize_ = 0;
    compressedUnmappedQualSize_ = 0;

    // Write block parameters
    compressedMappedQualSize_ += cqFile->writeUint32(posOffset_);
    compressedMappedQualSize_ += cqFile->writeUint32((uint32_t)qualityValueOffset_);

    // Write inverse quantization LUTs
    compressedMappedQualSize_ += cqFile->writeQuantizers(quantizers_);

    // Write unmapped quality values
    unsigned char *uqv = (unsigned char *)unmappedQualityValues_.c_str();
    size_t uqvSize = unmappedQualityValues_.length();
    if (uqvSize > 0) {
        compressedUnmappedQualSize_ += cqFile->writeUint8(0x01);
        compressedUnmappedQualSize_ += cqFile->writeQualBlock(uqv, uqvSize);
    } else {
        compressedUnmappedQualSize_ += cqFile->writeUint8(0x00);
    }

    // Write mapped quantizer indices
    std::string mqiString("");
    for (auto const &mappedQuantizerIndex : mappedQuantizerIndices_) {
        mqiString += std::to_string(mappedQuantizerIndex);
    }
    unsigned char *mqi = (unsigned char *)mqiString.c_str();
    size_t mqiSize = mqiString.length();
    if (mqiSize > 0) {
        compressedMappedQualSize_ += cqFile->writeUint8(0x01);
        compressedMappedQualSize_ += cqFile->writeQualBlock(mqi, mqiSize);
    } else {
        compressedMappedQualSize_ += cqFile->writeUint8(0x00);
    }

    // Write mapped quality value indices
    for (int i = 0; i < NR_QUANTIZERS; ++i) {
        std::deque<int> mqviStream = mappedQualityValueIndices_[i];
        std::string mqviString("");
        for (auto const &mqviInt : mqviStream) {
            mqviString += std::to_string(mqviInt);
        }
        unsigned char *mqvi = (unsigned char *)mqviString.c_str();
        size_t mqviSize = mqviString.length();
        if (mqviSize > 0) {
            compressedMappedQualSize_ += cqFile->writeUint8(0x01);
            compressedMappedQualSize_ += cqFile->writeQualBlock(mqvi, mqviSize);
        } else {
            compressedMappedQualSize_ += cqFile->writeUint8(0x00);
        }
    }

    return compressedQualSize();
}

size_t QualEncoder::compressedMappedQualSize(void) const { return compressedMappedQualSize_; }
size_t QualEncoder::compressedUnmappedQualSize(void) const { return compressedUnmappedQualSize_; }
size_t QualEncoder::compressedQualSize(void) const { return (compressedMappedQualSize_ + compressedUnmappedQualSize_); }
size_t QualEncoder::nrMappedRecords(void) const { return nrMappedRecords_; }
size_t QualEncoder::nrUnmappedRecords(void) const { return nrUnmappedRecords_; }
size_t QualEncoder::nrRecords(void) const { return (nrMappedRecords_ + nrUnmappedRecords_); }
size_t QualEncoder::uncompressedMappedQualSize(void) const { return uncompressedMappedQualSize_; }
size_t QualEncoder::uncompressedUnmappedQualSize(void) const { return uncompressedUnmappedQualSize_; }
size_t QualEncoder::uncompressedQualSize(void) const { return (uncompressedMappedQualSize_ + uncompressedUnmappedQualSize_); }

void QualEncoder::encodeMappedQual(const SAMRecord &samRecord) {
    size_t cigarIdx = 0;
    size_t cigarLen = samRecord.cigar.length();
    size_t opLen = 0;  // length of current CIGAR operation
    size_t qualIdx = 0;
    size_t quantizerIndicesIdx = samRecord.posMin - posOffset_;

    for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
        if (isdigit(samRecord.cigar[cigarIdx])) {
            opLen = opLen*10 + (size_t)samRecord.cigar[cigarIdx] - (size_t)'0';
            continue;
        }

        switch (samRecord.cigar[cigarIdx]) {
        case 'M':
        case '=':
        case 'X':
            // Encode opLen quality values with computed quantizer indices
            for (size_t i = 0; i < opLen; i++) {
                int q = static_cast<int>(samRecord.qual[qualIdx++]) - qualityValueOffset_;
                int quantizerIndex = mappedQuantizerIndices_[quantizerIndicesIdx++];
                int qualityValueIndex = quantizers_.at(quantizerIndex).valueToIndex(q);
                mappedQualityValueIndices_.at(quantizerIndex).push_back(qualityValueIndex);
            }
            break;
        case 'I':
        case 'S':
            // Encode opLen quality values with max quantizer index
            for (size_t i = 0; i < opLen; i++) {
                int q = static_cast<int>(samRecord.qual[qualIdx++]) - qualityValueOffset_;
                int qualityValueIndex = quantizers_.at(QUANTIZER_IDX_MAX).valueToIndex(q);
                mappedQualityValueIndices_.at(QUANTIZER_IDX_MAX).push_back(qualityValueIndex);
            }
            break;
        case 'D':
        case 'N':
            quantizerIndicesIdx += opLen;
            break;  // do nothing as these bases are not present
        case 'H':
        case 'P':
            break;  // these have been clipped
        default:
            throwErrorException("Bad CIGAR string");
        }
        opLen = 0;
    }
}

void QualEncoder::encodeUnmappedQual(const std::string &qual) {
    unmappedQualityValues_ += qual;
}

}  // namespace calq

