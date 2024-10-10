/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_QUAL_ENCODER_H_
#define SRC_GENIE_QUALITY_CALQ_QUAL_ENCODER_H_

// -----------------------------------------------------------------------------

#include <chrono>
#include <deque>
#include <map>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

#include "genie/quality/calq/genotyper.h"
#include "genie/quality/calq/haplotyper.h"
#include "genie/quality/calq/quantizer.h"
#include "genie/quality/calq/record_pileup.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

struct EncodingOptions;
struct DecodingBlock;
enum struct Version;
// -----------------------------------------------------------------------------

struct EncodingRead {
    uint32_t posMin;
    uint32_t posMax;
    std::string qvalues;
    std::string cigar;
    std::string sequence;
    std::string reference;
};

// -----------------------------------------------------------------------------

class QualEncoder {
 public:
    explicit QualEncoder(const EncodingOptions& options, const std::map<int, Quantizer>& quant, DecodingBlock* out);
    ~QualEncoder();
    void addMappedRecordToBlock(EncodingRecord& record);
    void finishBlock();
    [[nodiscard]] size_t nrMappedRecords() const;

 private:
    void quantizeUntil(uint64_t pos);
    void encodeRecords(std::vector<EncodingRecord> records);
    void encodeMappedQual(const std::string& qvalues, const std::string& cigar, uint64_t pos);

 private:
    // Sizes & counters
    size_t nrMappedRecords_;
    size_t minPosUnencoded;

    int NR_QUANTIZERS;

    // Quality value offset for this block
    uint8_t qualityValueOffset_;

    // 0-based position offset of this block
    uint64_t posOffset_;

    // Pileup
    genie::quality::calq::RecordPileup recordPileup;

    Haplotyper haplotyper_;
    Genotyper genotyper_;
    DecodingBlock* out;

    // Quantizers
    std::map<int, Quantizer> quantizers_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_QUAL_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
