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

#include "Common/constants.h"
#include "Common/Exceptions.h"
#include "Common/log.h"
// #include "config.h"
#include "QualCodec/Quantizers/UniformQuantizer.h"
#include "QualCodec/Quantizers/UniformMinMaxQuantizer.h"

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

      genotyper_(polyploidy, qualityValueOffset, NR_QUANTIZERS),

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

void QualEncoder::addMappedRecordToBlock(const SAMRecord &samRecord) {
    /*if(chr_change) {
        posOffset_ = samRecord.posMin;
        samPileupDeque_.setPosMin(samRecord.posMin);
        samPileupDeque_.setPosMax(samRecord.posMax);
    }*/
    if (nrMappedRecords() == 0) {
        posOffset_ = samRecord.posMin;
        //printf("%d\n",posOffset_);
        samPileupDeque_.setPosMin(samRecord.posMin);
        samPileupDeque_.setPosMax(samRecord.posMax);
    }
    //printf("%d %d\n",samRecord.posMin,posOffset_);
    if (samRecord.posMax > samPileupDeque_.posMax()) {
        samPileupDeque_.setPosMax(samRecord.posMax);
    }

    samRecord.addToPileupQueue(&samPileupDeque_);
    samRecordDeque_.push_back(samRecord);
    while (samPileupDeque_.posMin() < samRecord.posMin) {
        int k = genotyper_.computeQuantizerIndex(samPileupDeque_.front().seq, samPileupDeque_.front().qual);
        mappedQuantizerIndices_.push_back(k);
        samPileupDeque_.pop_front();
    }
  
    while (samRecordDeque_.front().posMax < samPileupDeque_.posMin()) {
        encodeMappedQual(samRecordDeque_.front());
        samRecordDeque_.pop_front();
    }

    uncompressedMappedQualSize_ += samRecord.qual.length();
    nrMappedRecords_++;
}

void QualEncoder::finishBlock(void) {
    // Compute all remaining quantizers
    while (samPileupDeque_.empty() == false) {
        int k = genotyper_.computeQuantizerIndex(samPileupDeque_.front().seq, samPileupDeque_.front().qual);
        mappedQuantizerIndices_.push_back(k);
        samPileupDeque_.pop_front();
    }

    // Process all remaining records from queue
    while (samRecordDeque_.empty() == false) {
        encodeMappedQual(samRecordDeque_.front());
        samRecordDeque_.pop_front();
    }

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
    //std::cout << "\n" << uqvSize << "\n" << std::endl;
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
    // std::cout << "\n" << mqiString << "\n" << std::endl;

    unsigned char *mqi = (unsigned char *)mqiString.c_str();
    size_t mqiSize = mqiString.length();
    // std::cout << "MQI Length: " << mqiSize << std::endl;
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

        // std::cout << "MQVI Length: " << mqviSize << std::endl;
        // std::cout << mqviString << std::endl;

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
       //printf("%c\n",samRecord.cigar[cigarIdx]);
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
               int q = (int)samRecord.qual[qualIdx++] - qualityValueOffset_;
               int quantizerIndex = mappedQuantizerIndices_[quantizerIndicesIdx++];
               int qualityValueIndex = quantizers_.at(quantizerIndex).valueToIndex(q);

               mappedQualityValueIndices_.at(quantizerIndex).push_back(qualityValueIndex);
           }
           break;
       case 'I':
       case 'S':
           // Encode opLen quality values with max quantizer index
           for (size_t i = 0; i < opLen; i++) {
               int q = (int)samRecord.qual[qualIdx++] - qualityValueOffset_;
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

