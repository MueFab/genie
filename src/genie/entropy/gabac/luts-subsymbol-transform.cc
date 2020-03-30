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
    lutsO2 = std::vector<LutOrder2>(numSubsyms,
                                    std::vector<LutOrder1>(numAlphaSubsym,
                                                           getInitLutsOrder1(numAlphaSubsym))
                                   );
}

void LUTsSubSymbolTransformation::decodeLutOrder1(Reader &reader, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut) {
    uint32_t i, j;
    for(i=0; i<numAlphaSubsym; i++) {
        lut[i].numMaxElems = reader.readLutSymbol(codingSubsymSize);
        for(j=0; j<=lut[i].numMaxElems; j++) {
            lut[i].entries[j].value = reader.readLutSymbol(codingSubsymSize);
            lut[i].entries[j].index = j;
        }
    }
}

void LUTsSubSymbolTransformation::decodeLUTs(const paramcabac::SupportValues& supportVals, const paramcabac::StateVars& stateVars, Reader &reader) {

    uint8_t const codingSubsymSize = supportVals.getCodingSubsymSize();
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag());
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();

    if(numLuts == 0 || codingOrder < 1)
        return;

    if(codingOrder == 2)
        setupLutsO2(stateVars.getNumSubsymbols(), numAlphaSubsym);
    else if(codingOrder == 1)
        setupLutsO1(stateVars.getNumSubsymbols(), numAlphaSubsym);

    for(uint32_t s=0; s<numLuts; s++) {
        if(codingOrder == 2) {
            for(uint32_t k=0; k<numAlphaSubsym; k++) {
                decodeLutOrder1(reader, numAlphaSubsym, codingSubsymSize, lutsO2[s][k]);
            }
        } else if(codingOrder == 1) {
            decodeLutOrder1(reader, numAlphaSubsym, codingSubsymSize, lutsO1[s]);
        }
    }
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
