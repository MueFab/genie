/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the `EncodingConfiguration` and `IOConfiguration` structures for GABAC.
 *
 * This file defines the structures used to configure the GABAC encoder and decoder. The
 * `EncodingConfiguration` struct is used to specify the transformation operations to be
 * applied during entropy encoding and decoding. It encapsulates configurations for
 * descriptor subsequences, specifying their parameters and properties. The `IOConfiguration`
 * struct manages input and output stream settings, logging options, and word sizes.
 *
 * @details This file includes configuration details for managing GABAC transformations and
 * the corresponding input/output settings. It provides a standardized interface to interact
 * with the encoding and decoding processes, ensuring smooth integration with GABAC workflows.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONFIGURATION_H_
#define SRC_GENIE_ENTROPY_GABAC_CONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <random>
#include <string>
#include <vector>
#include "genie/entropy/gabac/config-manual.h"
#include "genie/entropy/gabac/encode-transformed-subseq.h"
#include "genie/entropy/gabac/stream-handler.h"
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data-block.h"
#include "genie/util/runtime-exception.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Configuration for encoding a GABAC subsequence.
 *
 * The `EncodingConfiguration` struct specifies which transformations to apply on a
 * GABAC subsequence during entropy encoding. It contains details about the descriptor
 * subsequence configuration, including word sizes, transformations, and other parameters.
 * The structure supports comparison operators and provides serialization to JSON format.
 */
struct EncodingConfiguration {
 public:
    /**
     * @brief Constructs a default `EncodingConfiguration` object.
     *
     * Initializes a default configuration with no specific transformation parameters.
     * This constructor is primarily used for setting up a blank configuration object.
     */
    EncodingConfiguration();

    /**
     * @brief Constructs a configuration for a given descriptor subsequence index.
     *
     * This constructor sets up a configuration specific to a descriptor subsequence
     * identified by its index. The configuration is tailored for the subsequence's
     * unique characteristics.
     *
     * @param sub Descriptor subsequence index to configure.
     */
    explicit EncodingConfiguration(core::GenSubIndex sub);

    /**
     * @brief Constructs a configuration from an existing subsequence.
     *
     * Takes an existing `paramcabac::Subsequence` object and uses it to create a new
     * `EncodingConfiguration` instance.
     *
     * @param _subseq Existing subsequence configuration to use.
     */
    explicit EncodingConfiguration(paramcabac::Subsequence&& _subseq);

    /**
     * @brief Destructor for the `EncodingConfiguration` structure.
     *
     * Ensures that any resources held by the configuration object are properly released.
     */
    ~EncodingConfiguration();

    /**
     * @brief Equality comparison operator.
     * @param conf The configuration to compare against.
     * @return `true` if the configurations are identical, `false` otherwise.
     */
    bool operator==(const EncodingConfiguration& conf) const;

    /**
     * @brief Inequality comparison operator.
     * @param conf The configuration to compare against.
     * @return `true` if the configurations differ, `false` otherwise.
     */
    bool operator!=(const EncodingConfiguration& conf) const;

    /**
     * @brief Retrieves the word size for the subsequence.
     *
     * This method returns the size of each word in the subsequence, measured in bytes.
     *
     * @return The word size of the subsequence.
     */
    [[nodiscard]] uint8_t getSubseqWordSize() const;

    /**
     * @brief Retrieves the current subsequence configuration.
     *
     * Returns a constant reference to the encapsulated `paramcabac::Subsequence` object.
     *
     * @return The current subsequence configuration.
     */
    [[nodiscard]] const paramcabac::Subsequence& getSubseqConfig() const;

    /**
     * @brief Sets the subsequence configuration.
     *
     * Replaces the current configuration with a new `paramcabac::Subsequence` object.
     *
     * @param _subseqCfg The new subsequence configuration to set.
     */
    void setSubseqConfig(paramcabac::Subsequence&& _subseqCfg);

    /**
     * @brief Constructs an `EncodingConfiguration` from a JSON object.
     *
     * Initializes the configuration using values from a JSON object representation.
     *
     * @param j The JSON object containing configuration parameters.
     */
    explicit EncodingConfiguration(nlohmann::json j);

    /**
     * @brief Serializes the configuration to a JSON object.
     *
     * Converts the current configuration into a JSON object for easy serialization.
     *
     * @return The configuration represented as a JSON object.
     */
    [[nodiscard]] nlohmann::json toJson() const;

 private:
    paramcabac::Subsequence subseqCfg;  //!< Configuration details for the subsequence.
};

/**
 * @brief Specifies input and output stream configurations for GABAC.
 *
 * The `IOConfiguration` struct is used to manage I/O streams, word sizes, block sizes,
 * and logging options for the GABAC encoder and decoder. It also provides utilities
 * for validating streams and setting log levels.
 */
struct IOConfiguration {
    std::istream* inputStream;       //!< Pointer to the input stream for reading data.
    uint8_t inputWordsize;           //!< Size of each word in the input stream.
    std::istream* dependencyStream;  //!< Pointer to an optional dependency stream for reading.
    std::ostream* outputStream;      //!< Pointer to the output stream for writing data.
    uint8_t outputWordsize;          //!< Size of each word in the output stream.
    size_t blocksize;                //!< Block size in bytes for reading/writing. Set to 0 for reading all at once.

    std::ostream* logStream;  //!< Pointer to the stream for logging information.

    /**
     * @brief Defines the various logging levels.
     *
     * The `LogLevel` enum represents different levels of logging verbosity,
     * ranging from detailed tracing to fatal errors.
     */
    enum class LogLevel {
        LOG_TRACE = 0,    //!< Trace-level logging for detailed information.
        LOG_DEBUG = 1,    //!< Debug-level logging for intermediate results.
        LOG_INFO = 2,     //!< Info-level logging for general messages.
        LOG_WARNING = 3,  //!< Warning-level logging for suspicious events.
        LOG_ERROR = 4,    //!< Error-level logging for handled errors.
        LOG_FATAL = 5     //!< Fatal-level logging for critical errors.
    };
    LogLevel level;  //!< Selected logging level for the current configuration.

    /**
     * @brief Retrieves the appropriate logging stream for a given level.
     *
     * Returns the logging stream if the provided level is higher than or equal to the
     * currently configured logging level. Otherwise, returns a null stream.
     *
     * @param l The desired logging level for the message.
     * @return The corresponding output stream for logging.
     */
    [[nodiscard]] std::ostream& log(const LogLevel& l) const;

    /**
     * @brief Validates that all configured streams are open and working.
     *
     * This function checks if the input, output, and logging streams are properly
     * set up and operational. Throws an exception if any stream is in an invalid state.
     */
    void validate() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
