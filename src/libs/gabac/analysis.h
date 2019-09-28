/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#ifndef GABAC_ANALYSIS_H_
#define GABAC_ANALYSIS_H_

namespace gabac {

struct AnalysisConfiguration;
struct IOConfiguration;

const AnalysisConfiguration& getCandidateConfig();

void analyze(const IOConfiguration& ioconf, const AnalysisConfiguration& aconf);

}  // namespace gabac

#endif  // GABAC_ANALYSIS_H_
