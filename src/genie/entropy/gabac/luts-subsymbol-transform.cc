/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "luts-subsymbol-transform.h"

#include <algorithm>
#include <vector>
#include <functional>

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

LUTsSubSymbolTransformation::LUTsSubSymbolTransformation(const paramcabac::SupportValues& _supportVals,
                                                         const paramcabac::StateVars& _stateVars,
                                                         const bool _modeFlag)
    : supportVals(_supportVals),
      stateVars(_stateVars),
      encodingModeFlag(_modeFlag) {}

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

void LUTsSubSymbolTransformation::buildLuts(util::DataBlock* const symbols) {
    assert(symbols != nullptr);

    size_t numSymbols = symbols->size();
    if (numSymbols <= 0) return;

    uint8_t const outputSymbolSize = supportVals.getOutputSymbolSize();
    uint8_t const codingSubsymSize = supportVals.getCodingSubsymSize();
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag());
    uint8_t const numPrvs = stateVars.getNumPrvs(codingOrder,
                                                 supportVals.getShareSubsymPrvFlag());
    uint8_t const numSubsymbols = stateVars.getNumSubsymbols();
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();
    uint64_t const subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize)-1;

    if(numLuts == 0 || codingOrder < 1)
        return;

    util::BlockStepper r = symbols->getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    if(codingOrder == 2)
        setupLutsO2(numSubsymbols, numAlphaSubsym);
    else
        setupLutsO1(numSubsymbols, numAlphaSubsym);

    while (r.isValid()) {
        // Split symbol into subsymbols and then build subsymbols
        uint64_t symbolValue = r.get();
        uint64_t subsymValue = 0;
        uint32_t oss = outputSymbolSize;

        uint64_t symValue = abs((int64_t) symbolValue);
        for (uint8_t s=0; s<numSubsymbols; s++) {
            uint8_t lutIdx = (numLuts > 1) ? s : 0; // either private or shared LUT
            uint8_t prvIdx = (numPrvs > 1) ? s : 0; // either private or shared PRV

            subsymValue = (symValue>>(oss-=codingSubsymSize)) & subsymMask;

            if(codingOrder == 2) {
                LutOrder2 lut = lutsO2[lutIdx];
                lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[subsymValue].freq++;
                lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[subsymValue].value = subsymValue;
            } else if(codingOrder == 1) {
                LutOrder1 lut = lutsO1[lutIdx];
                lut[subsymbols[prvIdx].prvValues[0]].entries[subsymValue].freq++;
                lut[subsymbols[prvIdx].prvValues[0]].entries[subsymValue].value = subsymValue;
            }
        }

        r.inc();
    }
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

void LUTsSubSymbolTransformation::decodeLUTs(Reader &reader) {

    if(encodingModeFlag)
        return;

    uint8_t const codingSubsymSize = supportVals.getCodingSubsymSize();
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag());
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();

    if(numLuts == 0 || codingOrder < 1)
        return;

    if(codingOrder == 2) {
        setupLutsO2(stateVars.getNumSubsymbols(), numAlphaSubsym);
        for(uint32_t s=0; s<numLuts; s++) {
            for(uint32_t k=0; k<numAlphaSubsym; k++) {
                decodeLutOrder1(reader, numAlphaSubsym, codingSubsymSize, lutsO2[s][k]);
            }
        }
    }
    else if(codingOrder == 1) {
        setupLutsO1(stateVars.getNumSubsymbols(), numAlphaSubsym);
        for(uint32_t s=0; s<numLuts; s++) {
            decodeLutOrder1(reader, numAlphaSubsym, codingSubsymSize, lutsO1[s]);
        }
    }
}

void LUTsSubSymbolTransformation::sortLutRow(LutRow& lutRow) {
    // sort entries in descending order and populate numMaxElems;
    sort(lutRow.entries.begin(), lutRow.entries.end(), std::greater<LutEntry>());
    lutRow.numMaxElems = std::count_if(lutRow.entries.begin(), lutRow.entries.end(), [](LutEntry e){return e.freq != 0;});
}

void LUTsSubSymbolTransformation::encodeLutOrder1(Writer &writer, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut) {
    uint32_t i, j;
    for(i=0; i<numAlphaSubsym; i++) {
        sortLutRow(lut[i]);
        writer.writeLutSymbol(lut[i].numMaxElems, codingSubsymSize);
        for(j=0; j<=lut[i].numMaxElems; j++) {
            writer.writeLutSymbol(lut[i].entries[j].value, codingSubsymSize);
        }
    }
}

void LUTsSubSymbolTransformation::encodeLUTs(Writer &writer, util::DataBlock* const symbols) {

    if(!encodingModeFlag)
        return;

    uint8_t const codingSubsymSize = supportVals.getCodingSubsymSize();
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag());
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();

    if(numLuts == 0 || codingOrder < 1)
        return;

    // build LUTs from symbols
    buildLuts(symbols);

    // encode LUTs
    if(codingOrder == 2) {
        for(uint32_t s=0; s<numLuts; s++) {
            for(uint32_t k=0; k<numAlphaSubsym; k++) {
                encodeLutOrder1(writer, numAlphaSubsym, codingSubsymSize, lutsO2[s][k]);
            }
        }
    } else if(codingOrder == 1) {
        for(uint32_t s=0; s<numLuts; s++) {
            encodeLutOrder1(writer, numAlphaSubsym, codingSubsymSize, lutsO1[s]);
        }
    }
}

void LUTsSubSymbolTransformation::transform(std::vector<Subsymbol>& subsymbols,
                                            const uint8_t subsymIdx,
                                            const uint8_t lutIdx,
                                            const uint8_t prvIdx) {
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();

    Subsymbol subsymbol = subsymbols[subsymIdx];
    if(codingOrder == 2) {
        LutOrder2 lut = lutsO2[lutIdx];
        /* Search the index for subsymValue */
        for(uint64_t j=0; j<numAlphaSubsym; j++) {
            if(lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[j].value == subsymbol.subsymValue &&
               lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[j].freq > 0) {
                subsymbol.lutNumMaxElems = lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].numMaxElems;
                subsymbol.lutEntryIdx = j;
                break;
            }
        }
    } else if(codingOrder == 1) {
        /* Search the index for subsymValue */
        for(uint64_t j=0; j<numAlphaSubsym; j++) {
            LutOrder1 lut = lutsO1[lutIdx];
            if(lut[subsymbols[prvIdx].prvValues[0]].entries[j].value == subsymbol.subsymValue &&
               lut[subsymbols[prvIdx].prvValues[0]].entries[j].freq > 0) {
                subsymbol.lutNumMaxElems = lut[subsymbols[prvIdx].prvValues[0]].numMaxElems;
                subsymbol.lutEntryIdx = j;
                break;
            }
        }
    }
}

void LUTsSubSymbolTransformation::transform(util::DataBlock* const symbolsIn, util::DataBlock* const symbolsOut) {
    assert(symbolsIn != nullptr);

    size_t numSymbols = symbolsIn->size();
    if (numSymbols <= 0) return;

    uint8_t const outputSymbolSize = supportVals.getOutputSymbolSize();
    uint8_t const codingSubsymSize = supportVals.getCodingSubsymSize();
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint8_t const numLuts = stateVars.getNumLuts(codingOrder,
                                                 supportVals.getShareSubsymLutFlag());
    uint8_t const numPrvs = stateVars.getNumPrvs(codingOrder,
                                                 supportVals.getShareSubsymPrvFlag());
    uint8_t const numSubsymbols = stateVars.getNumSubsymbols();
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();
    uint64_t const subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize)-1;

    if(numLuts == 0 || codingOrder < 1)
        return;

    util::BlockStepper r = symbolsIn->getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    while (r.isValid()) {
        // Split symbol into subsymbols and then build subsymbols
        uint64_t symbolValue = r.get();
        uint64_t subsymValue = 0;
        uint32_t oss = outputSymbolSize;

        uint64_t symValue = abs((int64_t) symbolValue);
        for (uint8_t s=0; s<numSubsymbols; s++) {
            uint8_t lutIdx = (numLuts > 1) ? s : 0; // either private or shared LUT
            uint8_t prvIdx = (numPrvs > 1) ? s : 0; // either private or shared PRV

            subsymValue = (symValue>>(oss-=codingSubsymSize)) & subsymMask;

            if(codingOrder == 2) {
                LutOrder2 lut = lutsO2[lutIdx];
                /* Search the index for subsymValue */
                for(uint64_t j=0; j<numAlphaSubsym; j++) {
                    if(lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[j].value == subsymValue &&
                       lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[j].freq > 0) {
                        subsymbols[s].lutNumMaxElems = lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].numMaxElems;
                        subsymbols[s].lutEntryIdx = j;
                        break;
                    }
                }
            } else if(codingOrder == 1) {
                /* Search the index for subsymValue */
                for(uint64_t j=0; j<numAlphaSubsym; j++) {
                    LutOrder1 lut = lutsO1[lutIdx];
                    if(lut[subsymbols[prvIdx].prvValues[0]].entries[j].value == subsymValue &&
                       lut[subsymbols[prvIdx].prvValues[0]].entries[j].freq > 0) {
                        subsymbols[s].lutNumMaxElems = lut[subsymbols[prvIdx].prvValues[0]].numMaxElems;
                        subsymbols[s].lutEntryIdx = j;
                        break;
                    }
                }
            }
        }

        r.inc();
    }
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
