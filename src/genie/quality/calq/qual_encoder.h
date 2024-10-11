/**
 * @file qual_encoder.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the QualEncoder class for encoding quality values.
 *
 * This file contains the declaration of the `QualEncoder` class, which is responsible for
 * encoding mapped and unmapped genomic quality values using quantization techniques. It
 * organizes input quality values into structured blocks that can be compressed and encoded
 * efficiently, while keeping track of genomic metadata such as read positions, sequences,
 * and alignment details.
 *
 * @details The `QualEncoder` class uses an internal set of quantizers to determine the best
 * representation for the quality values of the input sequences. It handles both aligned and
 * unaligned quality values, and encodes the values based on the structure of the genomic
 * data provided. This class also tracks the number of records being encoded and performs
 * quantization for each quality block until a specified position.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_QUAL_ENCODER_H_
#define SRC_GENIE_QUALITY_CALQ_QUAL_ENCODER_H_

#include <chrono>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include "genie/quality/calq/genotyper.h"
#include "genie/quality/calq/haplotyper.h"
#include "genie/quality/calq/quantizer.h"
#include "genie/quality/calq/record_pileup.h"

namespace genie::quality::calq {

// Forward declarations
struct EncodingOptions;
struct DecodingBlock;
enum struct Version;

/**
 * @brief Structure to store metadata for encoding a single read.
 *
 * The `EncodingRead` structure keeps track of metadata necessary for encoding
 * a specific read, including its position, quality values, sequence, and alignment
 * information. It is used by the `QualEncoder` to organize the quality values into
 * encoding blocks.
 */
struct EncodingRead {
    uint32_t posMin;            //!< @brief The minimum 1-based position of the read in the genome.
    uint32_t posMax;            //!< @brief The maximum 1-based position of the read in the genome.
    std::string qvalues;        //!< @brief String representation of the quality values for this read.
    std::string cigar;          //!< @brief The CIGAR string describing the alignment of the read.
    std::string sequence;       //!< @brief The sequence of bases in this read.
    std::string reference;      //!< @brief The reference sequence corresponding to this read.
};

/**
 * @brief The main class for encoding quality values into blocks.
 *
 * The `QualEncoder` class is responsible for processing genomic quality values,
 * organizing them into blocks, and encoding them using quantizers and encoding strategies.
 * It uses the `Haplotyper` and `Genotyper` classes to analyze and transform the quality
 * values based on genomic context and side information. The encoded results are stored
 * in a `DecodingBlock` structure for further processing or storage.
 */
class QualEncoder {
 public:
    /**
     * @brief Constructs a `QualEncoder` object with the specified options and quantizers.
     *
     * Initializes the encoder with a set of encoding options and quantizers to be used
     * for encoding quality values. The encoded results are stored in the `DecodingBlock`
     * pointed to by the `out` parameter.
     *
     * @param options The encoding options for quality value processing.
     * @param quant A map of quantizers to be used for encoding.
     * @param out Pointer to the output structure where encoded quality values will be stored.
     */
    explicit QualEncoder(const EncodingOptions& options, const std::map<int, Quantizer>& quant, DecodingBlock* out);

    /**
     * @brief Destructor for the `QualEncoder` class.
     *
     * Cleans up the internal states and pointers used by the `QualEncoder`.
     */
    ~QualEncoder();

    /**
     * @brief Adds a mapped record to the current encoding block.
     *
     * This function adds a new record, specified by the `EncodingRecord` structure,
     * to the current block being encoded. The record's quality values, CIGAR string,
     * and sequence are used to build a compressed representation in the output block.
     *
     * @param record The `EncodingRecord` structure containing quality values and metadata for the read.
     */
    void addMappedRecordToBlock(EncodingRecord& record);

    /**
     * @brief Finalizes the current encoding block.
     *
     * This function finishes the current block by quantizing any remaining data and
     * storing the results in the output `DecodingBlock`.
     */
    void finishBlock();

    /**
     * @brief Returns the number of mapped records processed so far.
     *
     * @return The number of mapped records in the current block.
     */
    [[nodiscard]] size_t nrMappedRecords() const;

 private:
    /**
     * @brief Quantizes quality values up to the specified position.
     *
     * This function performs quantization on the quality values until the given
     * position is reached, transforming the values into a compact representation
     * that can be stored in the output block.
     *
     * @param pos The position up to which quantization should be performed.
     */
    void quantizeUntil(uint64_t pos);

    /**
     * @brief Encodes a set of records into the current block.
     *
     * This function takes a vector of `EncodingRecord` structures and encodes them
     * into the current block using the specified quantizers and options.
     *
     * @param records The records to be encoded in the current block.
     */
    void encodeRecords(std::vector<EncodingRecord> records);

    /**
     * @brief Encodes the quality values of a single mapped read.
     *
     * Encodes the quality values of a read, using the specified CIGAR string and
     * the read's position. The results are stored in the output block.
     *
     * @param qvalues The quality values to be encoded.
     * @param cigar The CIGAR string describing the alignment of the read.
     * @param pos The position of the read in the genome.
     */
    void encodeMappedQual(const std::string& qvalues, const std::string& cigar, uint64_t pos);

 private:
    size_t nrMappedRecords_;        //!< @brief Counter for the number of mapped records in the current block.
    size_t minPosUnencoded;         //!< @brief The minimum position of unencoded quality values.

    int NR_QUANTIZERS;              //!< @brief Number of quantizers available for encoding.

    uint8_t qualityValueOffset_;    //!< @brief Quality value offset for this block (e.g., ASCII offset).

    uint64_t posOffset_;            //!< @brief 0-based position offset for the current encoding block.

    RecordPileup recordPileup;      //!< @brief Structure to maintain the pileup of records for encoding.

    Haplotyper haplotyper_;         //!< @brief Haplotyper for encoding haplotype-specific quality values.
    Genotyper genotyper_;           //!< @brief Genotyper for calculating genotype probabilities.

    DecodingBlock* out;             //!< @brief Pointer to the output structure for storing encoded quality values.

    std::map<int, Quantizer> quantizers_;  //!< @brief Map of quantizers used for encoding quality values.
};

}  // namespace genie::quality::calq

#endif  // SRC_GENIE_QUALITY_CALQ_QUAL_ENCODER_H_

