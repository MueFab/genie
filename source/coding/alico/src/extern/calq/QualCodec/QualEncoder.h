/** @file QualEncoder.h
 *  @brief This file contains the definition of the QualEncoder class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_QUALCODEC_QUALENCODER_H_
#define CALQ_QUALCODEC_QUALENCODER_H_

#define QUANTIZER_STEPS_MIN 2
#define QUANTIZER_STEPS_MAX 8
#define NR_QUANTIZERS (QUANTIZER_STEPS_MAX-QUANTIZER_STEPS_MIN+1)
#define QUANTIZER_IDX_MIN 0
#define QUANTIZER_IDX_MAX (NR_QUANTIZERS-1)

#include <chrono>
#include <deque>
#include <map>
#include <string>
#include <vector>

// #include "config.h"
#include "IO/CQ/CQFile.h"
#include "IO/SAM/SAMPileupDeque.h"
#include "IO/SAM/SAMRecord.h"
#include "QualCodec/Genotyper.h"
#include "QualCodec/Quantizers/Quantizer.h"

namespace calq {

class QualEncoder {
 public:
    explicit QualEncoder(const int &polyploidy,
                         const int &qualityValueMax,
                         const int &qualityValueMin,
                         const int &qualityValueOffset);
    ~QualEncoder(void);

    void addUnmappedRecordToBlock(const SAMRecord &samRecord);
    void addMappedRecordToBlock(const SAMRecord &samRecord);
    void finishBlock(void);
    size_t writeBlock(CQFile *cqFile);

    size_t compressedMappedQualSize(void) const;
    size_t compressedUnmappedQualSize(void) const;
    size_t compressedQualSize(void) const;
    size_t nrMappedRecords(void) const;
    size_t nrUnmappedRecords(void) const;
    size_t nrRecords(void) const;
    size_t uncompressedMappedQualSize(void) const;
    size_t uncompressedUnmappedQualSize(void) const;
    size_t uncompressedQualSize(void) const;

 private:
    void encodeMappedQual(const SAMRecord &samRecord);
    void encodeUnmappedQual(const std::string &qual);

 private:
    // Sizes & counters
    size_t compressedMappedQualSize_;
    size_t compressedUnmappedQualSize_;
    size_t nrMappedRecords_;
    size_t nrUnmappedRecords_;
    size_t uncompressedMappedQualSize_;
    size_t uncompressedUnmappedQualSize_;

    // Quality value offset for this block
    int qualityValueOffset_;

    // 0-based position offset of this block
    uint32_t posOffset_;

    // Buffers
    std::string unmappedQualityValues_;
    std::deque<int> mappedQuantizerIndices_;
    std::vector< std::deque<int> > mappedQualityValueIndices_;

    // Pileup
    SAMPileupDeque samPileupDeque_;

    // Genotyper
    Genotyper genotyper_;

    // Quantizers
    std::map<int, Quantizer> quantizers_;

    // Double-ended queue holding the SAM records; records get popped when they
    // are finally encoded
    std::deque<SAMRecord> samRecordDeque_;
};

}  // namespace calq

#endif  // CALQ_QUALCODEC_QUALENCODER_H_

