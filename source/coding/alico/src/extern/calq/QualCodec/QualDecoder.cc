/** @file QualDecoder.cc
 *  @brief This file contains the implementation of the QualDecoder class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "QualCodec/QualDecoder.h"

#include <map>
#include <string>

#include "Common/Exceptions.h"
#include "Common/log.h"

namespace calq {

static size_t readLength(const std::string &cigar) {
    size_t readLen = 0;
    size_t cigarIdx = 0;
    size_t cigarLen = cigar.length();
    size_t opLen = 0;  // length of current CIGAR operation

    for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
       if (isdigit(cigar[cigarIdx])) {
           opLen = opLen*10 + (size_t)cigar[cigarIdx] - (size_t)'0';
           continue;
       }

       switch (cigar[cigarIdx]) {
       case 'M':
       case '=':
       case 'X':
           readLen += opLen;
           break;
       case 'I':
       case 'S':
           readLen += opLen;
           break;
       case 'D':
       case 'N':
           break;  // do nothing as these bases are not present
       case 'H':
       case 'P':
           break;  // these have been clipped
       default:
           throwErrorException("Bad CIGAR string");
       }

       opLen = 0;
    }

    return readLen;
}

QualDecoder::QualDecoder(void)
    : posOffset_(0),
      qualityValueOffset_(0),
      uqv_(""),
      qvci_(""),
      qvi_(),
      uqvIdx_(0),
      qviIdx_(),
      quantizers_() {}

QualDecoder::~QualDecoder(void) {}

void QualDecoder::decodeMappedRecordFromBlock(const SAMRecord &samRecord, File *qualFile) {
    std::string qual("");

    size_t cigarIdx = 0;
    size_t cigarLen = samRecord.cigar.length();
    size_t opLen = 0;
    //printf("%d %d\n",samRecord.posMin, posOffset_);
    size_t qvciPos = samRecord.posMin - posOffset_;
    
    for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
       //printf("%c\n",samRecord.cigar[cigarIdx]);
       if (isdigit(samRecord.cigar[cigarIdx])) {
           //printf("%d\n",samRecord.cigar.size());
           opLen = opLen*10 + (size_t)samRecord.cigar[cigarIdx] - (size_t)'0';
           continue;
       }
       //printf("%c\n",samRecord.cigar[cigarIdx]);
       switch (samRecord.cigar[cigarIdx]) {
       case 'M':
       case '=':
       case 'X':
           // Decode opLen quality value indices with computed quantizer indices

           for (size_t i = 0; i < opLen; i++) {
              //printf("%d\n",opLen);
              int quantizerIndex = qvci_[qvciPos++] - '0';
              //printf("%d\n",qvi_.size());
              //printf("%d\n",quantizerIndex);
               int qualityValueIndex = qvi_.at(quantizerIndex)[qviIdx_[quantizerIndex]++] - '0';
               int q = quantizers_.at(quantizerIndex).indexToReconstructionValue(qualityValueIndex);
               qual += q + qualityValueOffset_;
           }
           break;
       case 'I':
       case 'S':
           // Decode opLen quality values with max quantizer index
           for (size_t i = 0; i < opLen; i++) {
               int qualityValueIndex = qvi_.at(quantizers_.size() - 1)[qviIdx_[quantizers_.size() - 1]++] - '0';
               int q = quantizers_.at(quantizers_.size() - 1).indexToReconstructionValue(qualityValueIndex);
               qual += q + qualityValueOffset_;
           }
           break;
       case 'D':
       case 'N':
           qvciPos += opLen;
           break;  // do nothing as these bases are not present
       case 'H':
       case 'P':
           break;  // these have been clipped
       default:
           throwErrorException("Bad CIGAR string");
       }
       opLen = 0;
    }
    //printf("qual: %s\n", qual.c_str());
    qualFile->write((unsigned char *)qual.c_str(), qual.length());
    qualFile->writeByte('\n');
    
}

void QualDecoder::decodeUnmappedRecordFromBlock(const SAMRecord &samRecord, File *qualFile) {
    // Get the read length from SEQ (CIGAR might be unavailable for an
    // unmapped record)
    size_t qualLen = samRecord.seq.length();

    // Get the quality values
    std::string qual = uqv_.substr(uqvIdx_, qualLen);
    if (qual.empty() == true) {
        throwErrorException("Decoding quality values failed");
    }
    uqvIdx_ += qualLen;

    // Write the quality values
    // qualFile->write((unsigned char *)qual.c_str(), qual.length());
    // qualFile->writeByte('\n');
}

size_t QualDecoder::readBlock(CQFile *cqFile) {
    size_t ret = 0;

    // Read block parameters
    ret += cqFile->readUint32(&posOffset_);
    ret += cqFile->readUint32((uint32_t *)&qualityValueOffset_);

    // Read inverse quantization LUTs
    cqFile->readQuantizers(&quantizers_);

    // Read unmapped quality values
    uint8_t uqvFlags = 0;
    ret += cqFile->readUint8(&uqvFlags);
    if (uqvFlags & 0x01) {
        ret += cqFile->readQualBlock(&uqv_);
    }

    // Read mapped quantizer indices
    uint8_t mqiFlags = 0;
    ret += cqFile->readUint8(&mqiFlags);
    if (mqiFlags & 0x1) {
        ret += cqFile->readQualBlock(&qvci_);
    }

    // Read mapped quality value indices
    for (int i = 0; i < quantizers_.size(); ++i) {
        qvi_.push_back("");
        qviIdx_.push_back(0);
        uint8_t mqviFlags = 0;
        ret += cqFile->readUint8(&mqviFlags);
        if (mqviFlags & 0x1) {
            ret += cqFile->readQualBlock(&qvi_[i]);
        }
    }

    return ret;
}

}  // namespace calq

