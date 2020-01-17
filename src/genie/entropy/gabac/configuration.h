/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_CONFIGURATION_H_
#define GABAC_CONFIGURATION_H_

#include <istream>
#include <ostream>
#include <string>
#include <vector>

namespace genie {
namespace entropy {
namespace gabac {

enum class BinarizationId;
enum class ContextSelectionId;
enum class SequenceTransformationId;

/**
 * @brief Parameters for single stream
 */
struct TransformedSequenceConfiguration {
    bool lutTransformationEnabled;                    /**< @brief LUT transformation switch */
    unsigned int lutBits;                             /**< @brief Bits/values in LUT table */
    unsigned int lutOrder;                            /**< @brief Context size for LUT */
    bool diffCodingEnabled;                           /**< @brief Diff core switch */
    gabac::BinarizationId binarizationId;             /**< @brief Which binarization to use */
    std::vector<unsigned int> binarizationParameters; /**< @brief Parameters for binarization */
    gabac::ContextSelectionId contextSelectionId;     /**< @brief Which context to use in CABAC */

    /**
     * @brief Create a human readable string from this config
     * @return Text
     */
    std::string toPrintableString() const;

    /**
     * Compare
     * @param conf
     * @return
     */
    bool operator==(const TransformedSequenceConfiguration& conf) const;

    /**
     * Compare
     * @param conf
     * @return
     */
    bool operator!=(const TransformedSequenceConfiguration& conf) const;
};

/**
 * @brief Specifies which gabac transformations to execute
 */
struct EncodingConfiguration {
   public:
    /**
     * @brief Create default config
     */
    EncodingConfiguration();

    /**
     * @brief Create config from JSON
     * @param jsonstring JSON encoded configuration
     */
    explicit EncodingConfiguration(const std::string& jsonstring);

    /**
     * @brief Destroy config
     */
    ~EncodingConfiguration();

    /**
     * @brief Encode as json
     * @return JSON string
     */
    std::string toJsonString() const;

    /**
     * @brief Generate human readable text
     * @return Text
     */
    std::string toPrintableString() const;

    /**
     * Will modify this configuration so that it is guaranteed to work with the
     * parameter passed. It is also gueranteed that it will work with all
     * streams it was working before, even outside of the stream specification
     * passed.
     * @param max Maximum value in the stream
     * @param wordsize Maximum word size in the stream
     * @warning You can still break gabac if there is a sequence of more than
     * 4GBs of the same symbol while being in match core or run length core.
     * Moreover the modified config is probably not optimal, but it will work.
     * @return Adapted configuration
     */
    gabac::EncodingConfiguration generalize(uint64_t max, unsigned wordsize) const;

    /**
     * Applies generalize() and checks if it changed the configuration, i.a. if
     * it was generally applicable before.
     * @param max Maximum symbol
     * @param wordsize Maximum word size
     * @return True if working with all streams in specification
     */
    bool isGeneral(uint64_t max, unsigned wordsize) const;

    /**
     * Checks if a subsequence is general
     * @param max Maximum symbol
     * @param sub Subsequence id
     * @return true if general
     */
    bool isSubGeneral(uint64_t max, unsigned sub) const;

    /**
     * Will apply some small parameter tweaks that are guaranteed to improve
     * performance / speed regardless of the data in the stream. Other than
     * generalize() some streams might stop working if they are outside of the
     * specification. Streams inside the specifications are guaranteed to work
     * the same way they did before or better.
     * @param max Maximum symbol allowed in the stream
     * @param wordsize Maximum allowed word size
     * @return An improved configuration
     */
    gabac::EncodingConfiguration optimize(uint64_t max, unsigned wordsize) const;

    /**
     * Applies optimize() and checks if it changed the configuration, i.a. if it
     * was optimal before.
     * @param max Maximum symbol
     * @param wordsize Maximum word size
     * @return True if optimal
     */
    bool isOptimal(uint64_t max, unsigned wordsize) const;

    /**
     * Compare
     * @param conf
     * @return
     */
    bool operator==(const EncodingConfiguration& conf) const;

    /**
     * Compare
     * @param conf
     * @return
     */
    bool operator!=(const EncodingConfiguration& conf) const;

    unsigned int wordSize;                                    /**< @brief How many bytes are considered one symbol */
    gabac::SequenceTransformationId sequenceTransformationId; /**< @brief Which transformation to apply */
    unsigned int sequenceTransformationParameter;             /**< @brief Parameter for input stream
                                                                 transformation */
    std::vector<TransformedSequenceConfiguration> transformedSequenceConfigurations; /**< @brief Stream configs */
};

/**
 * @brief Specifies where to read and write.
 */
struct IOConfiguration {
    std::istream* inputStream;  /**< @brief Where to read from */
    std::ostream* outputStream; /**< @brief Where to write to */
    size_t blocksize;           /**< @brief How many bytes to read at once. Put 0 to read
                                   all in one go */

    std::ostream* logStream; /**< @brief Where to write logging information*/

    /**
     * @brief Logging level config
     */
    enum class LogLevel {
        TRACE = 0,   /**< @brief Log every step in great detail */
        DEBUG = 1,   /**< @brief Intermediate results */
        INFO = 2,    /**< @brief Expected Results */
        WARNING = 3, /**< @brief Suspicious events (may be an error) */
        ERROR = 4,   /**< @brief Handled errors */
        FATAL = 5    /**< @brief Error causing application to terminate */
    };
    LogLevel level; /**< @brief Selected level */

    /**
     * @brief Get a logging stream.
     * Returns the logging stream if the provided level is higher
     * than the currently selected level and a null stream otherwise
     * @param l Logging level of the message you want to write.
     * @return The appropriate stream
     * Usage: log(LogLevel::FATAL) << "N=NP" << std::endl;
     */
    std::ostream& log(const LogLevel& l) const;

    /**
     * @brief Check if all streams are open and working
     */
    void validate() const;
};

/**
 * @brief Candidates for analysis
 * TODO: Add json serialization
 */
struct AnalysisConfiguration {
    std::vector<unsigned> candidateWordsizes; /**< @brief Which word sizes to test */
    std::vector<gabac::SequenceTransformationId> candidateSequenceTransformationIds; /**<  @brief Transformationlist */
    std::vector<uint32_t> candidateMatchCodingParameters;                            /**< @brief Which match core window
                                                                                        sizes to test */
    std::vector<uint32_t> candidateRLECodingParameters;                  /**< @brief Which RLE guards to test */
    std::vector<bool> candidateLUTCodingParameters;                      /**< @brief Which LUT states
                                                                            (true, false) to test */
    std::vector<bool> candidateDiffParameters;                           /**< @brief Which diff states
                                                                            (true, false) to test */
    std::vector<gabac::BinarizationId> candidateUnsignedBinarizationIds; /**< @brief Which unsigned bins to
                                                                            test */
    std::vector<gabac::BinarizationId> candidateSignedBinarizationIds;   /**< @brief Which signed bins to test */
    std::vector<unsigned> candidateBinarizationParameters;               /**< @brief Which bin parameter to
                                                                            test */
    std::vector<gabac::ContextSelectionId> candidateContextSelectionIds; /**< @brief Which paramcabac contexts to test
                                                                          */
    std::vector<unsigned> candidateLutOrder;                             /**< @brief Which LUT orders to test */

    uint64_t maxValue;
    uint8_t wordSize;
};

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

#endif  // GABAC_CONFIGURATION_H_
