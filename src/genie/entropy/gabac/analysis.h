/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_ANALYSIS_H_
#define GABAC_ANALYSIS_H_

namespace genie {
namespace entropy {
namespace gabac {

struct AnalysisConfiguration;
struct IOConfiguration;

const AnalysisConfiguration& getCandidateConfig();

void analyze(const IOConfiguration& ioconf, const AnalysisConfiguration& aconf);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_ANALYSIS_H_
