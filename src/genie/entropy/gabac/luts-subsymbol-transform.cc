/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "luts-subsymbol-transform.h"

#include <algorithm>
#include <vector>

#include <genie/util/block-stepper.h>
#include <genie/util/data-block.h>
#include "exceptions.h"

namespace genie {
namespace entropy {
namespace gabac {

static inline
LutOrder1 getInitLutsOrder1(uint64_t numAlphaSubsym) {
    return std::vector<LutRow>(numAlphaSubsym,
                               {std::vector<LutEntry>(numAlphaSubsym,
                                                      {0,0,0}), // value, index, freq
                                0} // maxVaue
                              );
}

void LUTsSubSymbolTransformation::setupLutsO1(uint8_t numSubsyms, uint64_t numAlphaSubsym) {

    if(numAlphaSubsym > paramcabac::StateVars::MAX_LUT_SIZE)
        return;

    lutsO1.clear();
    lutsO1 = std::vector<LutOrder1>(numSubsyms,
                                    getInitLutsOrder1(numAlphaSubsym)
                                   );
}

void LUTsSubSymbolTransformation::setupLutsO2(uint8_t numSubsyms, uint64_t numAlphaSubsym) {

    if(numAlphaSubsym > paramcabac::StateVars::MAX_LUT_SIZE)
        return;

    lutsO2.clear();
    // Create a lutsO1 initialized with default values.
    //std::vector<LutOrder1> initLutsO1 = getInitLutsO1(numSubsyms, numAlphaSubsym);
    lutsO2 = std::vector<LutOrder2>(numSubsyms,
                                    std::vector<LutOrder1>(numAlphaSubsym,
                                                           getInitLutsOrder1(numAlphaSubsym))
                                   );
}

void LUTsSubSymbolTransformation::decodeLUTs(const paramcabac::SupportValues& supportVals, const paramcabac::StateVars& stateVars, Reader &reader) {

    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag());

    uint32_t s;
    setupLutsO1(stateVars.getNumSubsymbols(), stateVars.getNumAlphaSubsymbol());
    setupLutsO2(stateVars.getNumSubsymbols(), stateVars.getNumAlphaSubsymbol());

    for(s=0; s<numLuts; s++) {
        if(codingOrder == 2) {

        } else if(codingOrder == 1) {

        }
    }
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
