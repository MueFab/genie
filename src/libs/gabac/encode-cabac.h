/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_ENCODE_CABAC_H_
#define GABAC_ENCODE_CABAC_H_

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace util {
class DataBlock;
}

namespace genie {
namespace gabac {

enum class BinarizationId;
enum class ContextSelectionId;

void encode_cabac(const BinarizationId& binarizationId, const std::vector<uint32_t>& binarizationParameters,
                  const ContextSelectionId& contextSelectionId, util::DataBlock* symbols,
                  size_t maxsize = std::numeric_limits<size_t>::max());

}  // namespace gabac
}  // namespace genie
#endif  // GABAC_ENCODE_CABAC_H_
