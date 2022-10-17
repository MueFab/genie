/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
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

#include "genie/core/record/class-type.h"
#include "genie/quality/calq/genotyper.h"
#include "genie/quality/calq/haplotyper.h"
#include "genie/quality/calq/quantizer.h"
#include "genie/quality/calq/record_pileup.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

struct EncodingOptions;
struct DecodingBlock;
enum struct Version;
// -----------------------------------------------------------------------------

class QualEncoder {
 public:
    explicit QualEncoder(const EncodingOptions& options, const SideInformation& sideInfo,
                         const std::map<int, Quantizer>& quant, DecodingBlock* out);
    ~QualEncoder();
    void addMappedRecordToBlock(EncodingRecord& record);
    void finishBlock();
    size_t nrMappedRecords() const;

 private:
    void quantizeUntil(uint64_t pos);
    void encodeRecords(std::vector<EncodingRecord> records);
    void encodeMappedQual(const std::string& qvalues, const std::string& cigar, const uint64_t pos);

    // Sizes & counters
    size_t nrMappedRecords_;
    size_t minPosUnencoded;

    int NR_QUANTIZERS;

    bool hasUnalignedValues;

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

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_QUAL_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
