/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "diff-subsymbol-transform.h"

namespace genie {
namespace entropy {
namespace gabac {

void DiffSubSymbolTransform::invTransform(std::vector<Subsymbol>& subsymbols,
                                          const uint8_t subsymIdx,
                                          const uint8_t prvIdx) {
    subsymbols[subsymIdx].subsymValue += subsymbols[prvIdx].prvValues[0];
}

void DiffSubSymbolTransform::transform(std::vector<Subsymbol>& subsymbols,
                                          const uint8_t subsymIdx,
                                          const uint8_t prvIdx) {
    subsymbols[subsymIdx].subsymValue -= subsymbols[prvIdx].prvValues[0];
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie