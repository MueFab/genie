/** @file QualDecoder.h
 *  @brief This file contains the definition of the QualDecoder class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_QUALCODEC_QUALDECODER_H_
#define CALQ_QUALCODEC_QUALDECODER_H_

#include <map>
#include <string>
#include <vector>

#include "IO/CQ/CQFile.h"
#include "IO/SAM/SAMRecord.h"
#include "QualCodec/Quantizers/Quantizer.h"

namespace calq {

class QualDecoder {
 public:
    QualDecoder(void);
    ~QualDecoder(void);

    void decodeMappedRecordFromBlock(const SAMRecord &samRecord, File *qualFile);
    void decodeUnmappedRecordFromBlock(const SAMRecord &samRecord, File *qualFile);
    size_t readBlock(CQFile *cqFile);

 private:
    uint32_t posOffset_;
    int qualityValueOffset_;

    std::string uqv_;
    std::string qvci_;
    std::vector< std::string > qvi_;

    size_t uqvIdx_;
    std::vector< size_t > qviIdx_;

    std::map<int, Quantizer> quantizers_;
};

}  // namespace calq

#endif  // CALQ_QUALCODEC_QUALDECODER_H_

