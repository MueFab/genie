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

#include <genie/entropy/paramcabac/subsequence.h>

namespace genie {
namespace entropy {
namespace gabac {

#if 0
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

#endif

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


    paramcabac::Subsequence subseq;

    // RESTRUCT OBSOLETE To be removed
    //unsigned int wordSize;                                    /**< @brief How many bytes are considered one symbol */
    //gabac::SequenceTransformationId sequenceTransformationId; /**< @brief Which transformation to apply */
    //unsigned int sequenceTransformationParameter;             /**< @brief Parameter for input stream transformation */
    //std::vector<TransformedSequenceConfiguration> transformedSequenceConfigurations; /**< @brief Stream configs */
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

#if 0
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
#endif

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

#endif  // GABAC_CONFIGURATION_H_
