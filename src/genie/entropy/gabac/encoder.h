/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the GABAC Encoder class for compressing raw access units into block payloads.
 *
 * This file contains the declaration of the `Encoder` class, which extends the `EntropyEncoder` class.
 * It is responsible for encoding genomic data using the GABAC (Genomic Adaptive Binary Arithmetic Coding)
 * compression technique. The encoder performs the actual compression based on the configurations
 * specified and generates the compressed payloads that can be used for efficient storage and transmission.
 *
 * @details The `Encoder` class offers two main functions, `compress` and `compressTokens`, for transforming
 * genomic data based on the `EncodingConfiguration` provided. Additionally, the `process` method is overridden
 * to provide the entry point for compression operations when working with `AccessUnit::Descriptor` objects.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODER_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access-unit.h"              //!< Include for core AccessUnit definitions.
#include "genie/core/entropy-encoder.h"           //!< Include for the base EntropyEncoder class.
#include "genie/entropy/gabac/gabac-seq-conf-set.h"  //!< Include for handling GABAC configurations.
#include "genie/entropy/gabac/gabac.h"            //!< Include for core GABAC encoding functionalities.
#include "genie/util/stop-watch.h"                //!< Include for measuring compression time.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Module to compress raw access units into block payloads using GABAC.
 *
 * The `Encoder` class is a part of the GABAC library that provides functionalities to compress genomic data
 * using GABAC encoding techniques. It operates on raw access units, processes descriptors, and utilizes
 * predefined configurations to generate compressed block payloads that are suitable for storage and transmission.
 *
 * @details This class inherits from `core::EntropyEncoder` and implements the main interface method `process`
 * for compression. It also provides two static helper methods, `compress` and `compressTokens`, to perform
 * transformation on individual subsequences or descriptor tokens respectively.
 */
class Encoder : public core::EntropyEncoder {
 private:
    /**
     * @brief Perform GABAC compression on a single subsequence.
     *
     * This function takes a raw uncompressed subsequence and applies GABAC compression using the provided
     * configuration. The output is a transformed `AccessUnit::Subsequence` containing the compressed representation.
     *
     * @param conf GABAC configuration to use for compression.
     * @param in The input uncompressed subsequence.
     * @return A compressed `AccessUnit::Subsequence` that can be stored or transmitted.
     */
    static core::AccessUnit::Subsequence compress(const gabac::EncodingConfiguration& conf,
                                                  core::AccessUnit::Subsequence&& in);

    /**
     * @brief Perform GABAC compression on descriptor tokens.
     *
     * This function compresses the descriptor tokens of an access unit descriptor using the provided GABAC
     * configuration. It is primarily used for compressing specific fields or descriptors within the access unit.
     *
     * @param conf0 The configuration to use for token compression.
     * @param in The input uncompressed descriptor.
     * @return A compressed `AccessUnit::Descriptor` containing the transformed representation of the tokens.
     */
    static core::AccessUnit::Descriptor compressTokens(const gabac::EncodingConfiguration& conf0,
                                                       core::AccessUnit::Descriptor&& in);

 public:
    GabacSeqConfSet configSet;  //!< @brief Configuration set for GABAC compression. This set remains static over time.
    bool writeOutStreams{};     //!< @brief Flag to indicate whether to write out streams for debugging or analysis.

    /**
     * @brief Process and compress the given descriptor using GABAC encoding.
     *
     * This method overrides the `process` function from `core::EntropyEncoder` and provides the main
     * interface for compressing access unit descriptors. It applies the GABAC compression based on
     * the internal configuration set and returns the encoded result as an `EntropyCoded` object.
     *
     * @param desc The input descriptor to be compressed.
     * @return The compressed `EntropyCoded` representation of the input descriptor.
     */
    EntropyCoded process(core::AccessUnit::Descriptor& desc) override;

    /**
     * @brief Constructs an Encoder object with the option to enable or disable stream writing.
     *
     * This constructor initializes the encoder with a flag indicating whether the transformed streams
     * should be written out for debugging or analysis purposes.
     *
     * @param _writeOutStreams Flag to enable or disable writing out streams.
     */
    explicit Encoder(bool _writeOutStreams);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
