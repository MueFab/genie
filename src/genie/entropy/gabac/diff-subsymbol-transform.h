/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_DIFF_SUBSYMBOL_TRANSFORM_H_
#define GABAC_DIFF_SUBSYMBOL_TRANSFORM_H_

#include "subsymbol.h"

namespace genie {
namespace entropy {
namespace gabac {

class DiffSubSymbolTransform {
   public:
    DiffSubSymbolTransform() = default;

    ~DiffSubSymbolTransform() = default;

    static
    void invTransform(std::vector<Subsymbol>& subsymbols,
                      const uint8_t subsymIdx,
                      const uint8_t prvIdx);

    static
    void transform(std::vector<Subsymbol>& subsymbols,
                   const uint8_t subsymIdx,
                   const uint8_t prvIdx);
};

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_DIFF_SUBSYMBOL_TRANSFORM_H_
