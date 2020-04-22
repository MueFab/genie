#ifndef GABACIFY_CONFIG_MANUAL_H_
#define GABACIFY_CONFIG_MANUAL_H_

#include <cstddef>
#include <string>

#include <genie/entropy/gabac/gabac.h>
#include <genie/core/constants.h>

namespace gabacify {

using namespace genie::core;
using namespace genie::entropy::gabac;

EncodingConfiguration getEncoderConfigManual(
        uint8_t descID,
        uint8_t subseqID);

}  // namespace gabacify

#endif  // GABACIFY_CONFIG_MANUAL_H_
