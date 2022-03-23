/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONFIGURATION_H_
#define SRC_GENIE_ENTROPY_GABAC_CONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include "genie/entropy/gabac/config-manual.h"
#include "genie/entropy/paramcabac/subsequence.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

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
     * @brief Create default config for a given descriptor subsequence
     * @param sub descriptor subsequence index.
     */
    explicit EncodingConfiguration(const core::GenSubIndex sub);

    /**
     * @brief Create config from subseq
     */
    explicit EncodingConfiguration(paramcabac::Subsequence&& _subseq);

    /**
     * @brief Destroy config
     */
    ~EncodingConfiguration();

    /**
     * @brief Compare
     * @param conf
     * @return
     */
    bool operator==(const EncodingConfiguration& conf) const;

    /**
     * @brief Compare
     * @param conf
     * @return
     */
    bool operator!=(const EncodingConfiguration& conf) const;

    /**
     * @brief
     * @return
     */
    uint8_t getSubseqWordSize() const;

    /**
     * @brief
     * @return
     */
    const paramcabac::Subsequence& getSubseqConfig() const;

    /**
     * @brief
     * @param _subseqCfg
     */
    void setSubseqConfig(paramcabac::Subsequence&& _subseqCfg);

    /**
     * @brief
     * @param j
     */
    explicit EncodingConfiguration(nlohmann::json j);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

 private:
    paramcabac::Subsequence subseqCfg;  //!< @brief
};

/**
 * @brief Specifies where to read and write.
 */
struct IOConfiguration {
    std::istream* inputStream;      /**< @brief Where to read from */
    std::istream* dependencyStream; /**< @brief Where to read from */
    std::ostream* outputStream;     /**< @brief Where to write to */
    size_t blocksize;               /**< @brief How many bytes to read at once. Put 0 to read all in one go */

    std::ostream* logStream; /**< @brief Where to write logging information*/

    /**
     * @brief Logging level config
     */
    enum class LogLevel {
        LOG_TRACE = 0,   /**< @brief Log every step in great detail */
        LOG_DEBUG = 1,   /**< @brief Intermediate results */
        LOG_INFO = 2,    /**< @brief Expected Results */
        LOG_WARNING = 3, /**< @brief Suspicious events (may be an error) */
        LOG_ERROR = 4,   /**< @brief Handled errors */
        LOG_FATAL = 5    /**< @brief Error causing application to terminate */
    };
    LogLevel level; /**< @brief Selected level */

    /**
     * @brief Get a logging stream.
     * Returns the logging stream if the provided level is higher
     * than the currently selected level and a null stream otherwise
     * @param l Logging level of the message you want to write.
     * @return The appropriate stream
     * Usage: log(LogLevel::LOG_FATAL) << "N=NP" << std::endl;
     */
    std::ostream& log(const LogLevel& l) const;

    /**
     * @brief Check if all streams are open and working
     */
    void validate() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
