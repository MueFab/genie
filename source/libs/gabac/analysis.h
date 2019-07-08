/**
 * @file
 * @brief Access to the analyze function of gabac
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_ANALYSIS_H_
#define GABAC_ANALYSIS_H_

namespace gabac {

struct AnalysisConfiguration;
struct IOConfiguration;

/**
 * @brief Build a standard analysis configuration
 * @deprecated Should be removed in the future. Use json file instead
 * @return Default analysis config
 */
const AnalysisConfiguration& getCandidateConfig();

/**
 * @brief Find the best configuration for a data stream
 * @param ioconf I/O parameters
 * @param aconf Parameter search space
 */
void analyze(const IOConfiguration& ioconf,
             const AnalysisConfiguration& aconf
);

}  // namespace gabac


#endif  // GABAC_ANALYSIS_H_
