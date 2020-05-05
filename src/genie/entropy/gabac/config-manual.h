#ifndef GABAC_CONFIG_MANUAL_H_
#define GABAC_CONFIG_MANUAL_H_

#include <cstddef>
#include <string>

#include <genie/entropy/gabac/gabac.h>
#include <genie/core/constants.h>

namespace genie {
namespace entropy {
namespace gabac {

using namespace genie::core;

paramcabac::Subsequence getEncoderConfigManual(const core::GenSubIndex sub);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

#endif  // GABAC_CONFIG_MANUAL_H_
