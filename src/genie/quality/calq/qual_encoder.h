#ifndef CALQ_QUAL_ENCODER_H_
#define CALQ_QUAL_ENCODER_H_

// -----------------------------------------------------------------------------

#include <chrono>
#include <deque>
#include <map>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

#include "genotyper.h"
#include "haplotyper.h"
#include "quantizer.h"
#include "record_pileup.h"

// -----------------------------------------------------------------------------

namespace calq {

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
    size_t nrMappedRecords() const;

 private:
    void encodeMappedQual(const std::string& qvalues, const std::string& cigar, const uint32_t pos);

 private:
    // Sizes & counters
    size_t nrMappedRecords_;
    size_t minPosUnencoded;

    int NR_QUANTIZERS;

    // Quality value offset for this block
    uint8_t qualityValueOffset_;

    // 0-based position offset of this block
    uint32_t posOffset_;

    // Pileup
    genie::quality::calq::RecordPileup recordPileup;

    Haplotyper haplotyper_;
    Genotyper genotyper_;
    DecodingBlock* out;
    size_t posCounter;

    uint8_t hqSoftClipThreshold;

    // Quantizers
    std::map<int, Quantizer> quantizers_;

    // Double-ended queue holding the SAM records; records get popped when they
    // are finally encoded
    std::deque<EncodingRead> samRecordDeque_;

    Version version_;
};

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------

#endif  // CALQ_QUAL_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
