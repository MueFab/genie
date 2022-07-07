#ifndef CALQ_CALQ_CODER_H_
#define CALQ_CALQ_CODER_H_

// -----------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

// -----------------------------------------------------------------------------

#include "log.h"

// -----------------------------------------------------------------------------

namespace calq {

/**
 * Side information used to encode / decode quality values. It is not encoded
 * itself. All reads have to be mapped and sorted by their mapping position.
 */
struct SideInformation
{
    /**
     * 1-based leftmost mapping position for each read.
     * Needed for: Encoding, Decoding.
     */
    std::vector<uint32_t> positions;

    /**
     * Base sequence for each read.
     * Needed for: Encoding, Decoding (only using sequence lengths, so the
     * exact characters are not important)
     */
    std::vector<std::string> sequences;

    /**
     * Cigar string for each read.
     * Needed for: Encoding.
     */
    std::vector<std::string> cigars;

    /**
     * Reference sequence starting from the first mapped base through the last
     * mapped position. For calq V1 this cań remain empty. It is safe to have
     * additional bases at the end of the string, but not to have them in front.
     * Needed for: Encoding
     */
    std::string reference;

    /**
     * First position of read (including unmapped).
     * Needed for: Encoding, Decoding
     */
    uint32_t posOffset;

    /**
     * Quality value format offset.
     * Needed for: Encoding, Decoding
     */
    uint8_t qualOffset;
};

// -----------------------------------------------------------------------------

/**
 * Block of quality values to be encoded.
 */
struct EncodingBlock
{
    /**
     * Sequences of quality values for each read (mapped).
     */
    std::vector<std::string> qvalues;
};

// -----------------------------------------------------------------------------

/**
 * Quantizer types. Static uniform, adaptive lloyd quantizer.
 */
enum struct QuantizerType
{
    UNIFORM,
    LLOYD_MAX
};

// -----------------------------------------------------------------------------

/**
 * Filter for calq V2. Smooth gauss vs hard rectangle.
 */
enum struct FilterType
{
    GAUSS,
    RECTANGLE
};

// -----------------------------------------------------------------------------

/**
 * V1: Original Calq version. Based on genotype certainty, using no reference
 * V2: Based on variant probability, using reference
 */
enum struct Version
{
    V1,
    V2
};

// -----------------------------------------------------------------------------

/**
 * Additional parameters for encoding
 */
struct EncodingOptions
{
    /**
     * Output additional pileup information (warning: very verbose)
     */
    bool debugPileup = false;

    /**
     * Calq V2: Squash activity values between 0.0 and 1.0, so that they can be
     * treated as probabilities
     */
    bool squash = true;

    /**
     * Calq V2: Filter Radius. How big should the filter be? For gauss this
     * is sigma, for rect filter it is the radius similar to cutoff.
     */
    size_t filterSize = 17;

    /**
     * Calq V2: Filter Cutoff. How big should the filter kernel be? Infinite
     * filters like gauss will be cut off at that point. The value specified
     * here will be the radius of the kernel, so the kernel size is
     * filterCutOff*2 + 1.
     */
    size_t filterCutOff = 50;

    /**
     * Calq V2: Quality value (without offset) at which a soft clip is
     * considered "high quality"
     */
    uint8_t hqSoftClipThreshold = 29;

    /**
     * Calq V2: How many bases the activity values are supposed to be spread
     * once the streak limit has been reached
     */
    size_t hqSoftClipPropagation = 50;

    /**
     * Calq V2: How many high quality softclips there have to be at one position
     * to trigger the propagation
     */
    size_t hqSoftClipStreak = 7;


    /**
     * Lowest Quantization step number
     */
    uint8_t quantizationMin = 2;

    /**
     * Highest Quantization step number
     */
    uint8_t quantizationMax = 8;

    /**
     * Polyploidy of biological data source.
     */
    uint8_t polyploidy = 2;

    /**
     * Maximum quality value for the used format
     */
    uint8_t qualityValueMax = 41;

    /**
     * Minimum quality value for the used format
     */
    uint8_t qualityValueMin = 0;

    /**
     * Offset quality value for the used format
     */
    uint8_t qualityValueOffset = 33;

    /**
     * Calq V2: used filter to even out activity value
     */
    FilterType filterType = FilterType::GAUSS;

    /**
     * Quantizer used
     */
    QuantizerType quantizerType = QuantizerType::UNIFORM;

    /***
     * Calq version used
     */
    Version version = Version::V1;
};

// -----------------------------------------------------------------------------

/**
 * Decoding parameters
 */
struct DecodingOptions
{
};

// -----------------------------------------------------------------------------

/**
 * Encoded streams that can be decoded by calq.
 */
struct DecodingBlock
{
    /**
     *  Quantizer selection
     */
    std::vector<uint8_t> quantizerIndices;

    /**
     *  Step selection for each quantizer
     */
    std::vector<std::vector<uint8_t>> stepindices;

    /**
     *  Quantizer representative values
     */
    std::vector<std::vector<uint8_t>> codeBooks;
};

// -----------------------------------------------------------------------------

/**
 * Decode quality values.
 * @param sideInformation Alignment information.
 * @param input Encoded quality values
 * @param output Decoded quality values
 */
void decode(const DecodingOptions& opt,
            const SideInformation& sideInformation,
            const DecodingBlock& input,
            EncodingBlock *output
);

// -----------------------------------------------------------------------------

/**
 * Encode quality values.
 * @param opt Encoding options.
 * @param sideInformation Alignment information.
 * @param input Decoded quality values
 * @param output Encoded quality values
 */
void encode(const EncodingOptions& opt,
            const SideInformation& sideInformation,
            const EncodingBlock& input,
            DecodingBlock *output
);

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------

#endif  // CALQ_CALQ_CODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
