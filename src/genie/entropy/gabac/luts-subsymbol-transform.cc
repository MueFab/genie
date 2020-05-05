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

LutOrder1 LUTsSubSymbolTransform::getInitLutsOrder1(uint64_t numAlphaSubsym) {
    return std::vector<LutRow>(numAlphaSubsym,
                               {std::vector<LutEntry>(numAlphaSubsym,
                                                      {0,0}), // value, freq
                                0} // numMaxElems
                              );
}

LUTsSubSymbolTransform::LUTsSubSymbolTransform(const paramcabac::SupportValues& _supportVals,
                                               const paramcabac::StateVars& _stateVars,
                                               uint8_t _numLuts,
                                               uint8_t _numPrvs,
                                               const bool _modeFlag)
    : supportVals(_supportVals),
      stateVars(_stateVars),
      numLuts(_numLuts),
      numPrvs(_numPrvs),
      encodingModeFlag(_modeFlag) {}

void LUTsSubSymbolTransform::setupLutsOrder1(uint8_t numSubsyms, uint64_t numAlphaSubsym) {

    if(numAlphaSubsym > paramcabac::StateVars::MAX_LUT_SIZE)
        return;

    lutsO1.clear();
    lutsO1 = std::vector<LutOrder1>(numSubsyms,
                                    getInitLutsOrder1(numAlphaSubsym)
    );
}

void LUTsSubSymbolTransform::setupLutsOrder2(uint8_t numSubsyms, uint64_t numAlphaSubsym) {

    if(numAlphaSubsym > paramcabac::StateVars::MAX_LUT_SIZE)
        return;

    lutsO2.clear();
    lutsO2 = std::vector<LutOrder2>(numSubsyms,
                                    std::vector<LutOrder1>(numAlphaSubsym,
                                                           getInitLutsOrder1(numAlphaSubsym))
                                   );
}

void LUTsSubSymbolTransform::buildLuts(const core::Alphabet alphaProps, util::DataBlock* const symbols, util::DataBlock* const depSymbols) {
    if(numLuts == 0 || !encodingModeFlag)
        return;

    assert(symbols != nullptr);

    size_t numSymbols = symbols->size();
    if (numSymbols <= 0) return;

    uint8_t const outputSymbolSize = supportVals.getOutputSymbolSize();
    uint8_t const codingSubsymSize = supportVals.getCodingSubsymSize();
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint8_t const numSubsymbols = stateVars.getNumSubsymbols();
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();
    uint64_t const subsymMask = paramcabac::StateVars::get2PowN(codingSubsymSize)-1;

    util::BlockStepper r = symbols->getReader();
    std::vector<Subsymbol> subsymbols(stateVars.getNumSubsymbols());

    util::BlockStepper d;
    if(depSymbols) {
        d = depSymbols->getReader();
    }

    if(codingOrder == 2)
        setupLutsOrder2(numSubsymbols, numAlphaSubsym);
    else
        setupLutsOrder1(numSubsymbols, numAlphaSubsym);

    while (r.isValid()) {
        // Split symbol into subsymbols and then build subsymbols
        uint64_t symbolValue = r.get();
        uint64_t subsymValue = 0;
        uint32_t oss = outputSymbolSize;

        uint64_t depSymbolValue = 0, depSubsymValue = 0;
        if(d.isValid()) {
            depSymbolValue = alphaProps.inverseLut[d.get()];
            d.inc();
        }

        uint64_t symValue = abs((int64_t) symbolValue);
        for (uint8_t s=0; s<numSubsymbols; s++) {
            uint8_t lutIdx = (numLuts > 1) ? s : 0; // either private or shared LUT
            uint8_t prvIdx = (numPrvs > 1) ? s : 0; // either private or shared PRV

            if(depSymbols) {
                depSubsymValue = (depSymbolValue>>(oss-codingSubsymSize)) & subsymMask;
                subsymbols[prvIdx].prvValues[0] = depSubsymValue;
            }

            subsymValue = (symValue>>(oss-=codingSubsymSize)) & subsymMask;

            if(codingOrder == 2) {
                LutOrder2& lut = lutsO2[lutIdx];
                lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[subsymValue].freq++;
                lut[subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[subsymValue].value = subsymValue;

                subsymbols[prvIdx].prvValues[1] = subsymbols[prvIdx].prvValues[0];
                subsymbols[prvIdx].prvValues[0] = subsymValue;

            } else if(codingOrder == 1) {
                LutOrder1& lut = lutsO1[lutIdx];
                lut[subsymbols[prvIdx].prvValues[0]].entries[subsymValue].freq++;
                lut[subsymbols[prvIdx].prvValues[0]].entries[subsymValue].value = subsymValue;

                subsymbols[prvIdx].prvValues[0] = subsymValue;
            }
        }

        r.inc();
    }

    return;
}

void LUTsSubSymbolTransform::decodeLutOrder1(Reader &reader, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut) {
    uint32_t i, j;
    for(i=0; i<numAlphaSubsym; i++) {
        lut[i].numMaxElems = reader.readLutSymbol(codingSubsymSize);
        for(j=0; j<=lut[i].numMaxElems; j++) {
            lut[i].entries[j].value = reader.readLutSymbol(codingSubsymSize);
        }
    }
}

void LUTsSubSymbolTransform::decodeLUTs(Reader &reader) {

    if(numLuts == 0 || encodingModeFlag)
        return;

    uint8_t const codingSubsymSize = supportVals.getCodingSubsymSize();
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();

    if(codingOrder == 2) {
        setupLutsOrder2(stateVars.getNumSubsymbols(), numAlphaSubsym);
        for(uint32_t s=0; s<numLuts; s++) {
            for(uint32_t k=0; k<numAlphaSubsym; k++) {
                decodeLutOrder1(reader, numAlphaSubsym, codingSubsymSize, lutsO2[s][k]);
            }
        }
    }
    else if(codingOrder == 1) {
        setupLutsOrder1(stateVars.getNumSubsymbols(), numAlphaSubsym);
        for(uint32_t s=0; s<numLuts; s++) {
            decodeLutOrder1(reader, numAlphaSubsym, codingSubsymSize, lutsO1[s]);
        }
    }
}

void LUTsSubSymbolTransform::sortLutRow(LutRow& lutRow) {
    // sort entries in descending order and populate numMaxElems;
    sort(lutRow.entries.begin(), lutRow.entries.end(), std::greater_equal<LutEntry>());
    lutRow.numMaxElems = std::count_if(lutRow.entries.begin(), lutRow.entries.end(), [](LutEntry e){return e.freq != 0;});
    if(lutRow.numMaxElems > 0)
        lutRow.numMaxElems--;
}

void LUTsSubSymbolTransform::encodeLutOrder1(Writer &writer, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut) {
    uint32_t i, j;
    for(i=0; i<numAlphaSubsym; i++) {
        sortLutRow(lut[i]);
        writer.writeLutSymbol(lut[i].numMaxElems, codingSubsymSize);
        for(j=0; j<=lut[i].numMaxElems; j++) {
            writer.writeLutSymbol(lut[i].entries[j].value, codingSubsymSize);
        }
    }
}

void LUTsSubSymbolTransform::encodeLUTs(Writer &writer, const core::Alphabet alphaProps, util::DataBlock* const symbols, util::DataBlock* const depSymbols) {

    if(numLuts == 0 || !encodingModeFlag)
        return;

    uint8_t const codingSubsymSize = supportVals.getCodingSubsymSize();
    uint8_t const codingOrder = supportVals.getCodingOrder();
    uint64_t const numAlphaSubsym = stateVars.getNumAlphaSubsymbol();

    // build LUTs from symbols
    buildLuts(alphaProps, symbols, depSymbols);

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

void LUTsSubSymbolTransform::transformOrder2(std::vector<Subsymbol>& subsymbols,
                                             const uint8_t subsymIdx,
                                             const uint8_t lutIdx,
                                             const uint8_t prvIdx) {
    Subsymbol& subsymbol = subsymbols[subsymIdx];
    const uint64_t subsymVal = subsymbol.subsymValue;
    const LutRow lutRow = lutsO2[lutIdx][subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]];
    auto entry = std::find_if(lutRow.entries.begin(),
                              lutRow.entries.end(),
                              [subsymVal](LutEntry e){return e.value == subsymVal && e.freq > 0;});
    if(entry != lutRow.entries.end()) {
        subsymbol.lutNumMaxElems = lutRow.numMaxElems;
        subsymbol.lutEntryIdx = distance(lutRow.entries.begin(), entry);
    }
}

void LUTsSubSymbolTransform::transformOrder1(std::vector<Subsymbol>& subsymbols,
                                             const uint8_t subsymIdx,
                                             const uint8_t lutIdx,
                                             const uint8_t prvIdx) {
    Subsymbol& subsymbol = subsymbols[subsymIdx];
    const uint64_t subsymVal = subsymbol.subsymValue;
    const LutRow lutRow = lutsO1[lutIdx][subsymbols[prvIdx].prvValues[0]];

    auto entry = std::find_if(lutRow.entries.begin(),
                              lutRow.entries.end(),
                              [subsymVal](LutEntry e){return e.value == subsymVal && e.freq > 0;});
    if(entry != lutRow.entries.end()) {
        subsymbol.lutNumMaxElems = lutRow.numMaxElems;
        subsymbol.lutEntryIdx = distance(lutRow.entries.begin(), entry);
    }
}

uint64_t LUTsSubSymbolTransform::getNumMaxElemsOrder2(std::vector<Subsymbol>& subsymbols,
                                                      const uint8_t lutIdx,
                                                      const uint8_t prvIdx) {
    return lutsO2[lutIdx][subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].numMaxElems;
}

uint64_t LUTsSubSymbolTransform::getNumMaxElemsOrder1(std::vector<Subsymbol>& subsymbols,
                                                      const uint8_t lutIdx,
                                                      const uint8_t prvIdx) {
    return lutsO1[lutIdx][subsymbols[prvIdx].prvValues[0]].numMaxElems;
}

void LUTsSubSymbolTransform::invTransformOrder2(std::vector<Subsymbol>& subsymbols,
                                                const uint8_t subsymIdx,
                                                const uint8_t lutIdx,
                                                const uint8_t prvIdx) {
    subsymbols[subsymIdx].subsymValue = lutsO2[lutIdx][subsymbols[prvIdx].prvValues[1]][subsymbols[prvIdx].prvValues[0]].entries[subsymbols[subsymIdx].lutEntryIdx].value;
}

void LUTsSubSymbolTransform::invTransformOrder1(std::vector<Subsymbol>& subsymbols,
                                                const uint8_t subsymIdx,
                                                const uint8_t lutIdx,
                                                const uint8_t prvIdx) {
    subsymbols[subsymIdx].subsymValue = lutsO1[lutIdx][subsymbols[prvIdx].prvValues[0]].entries[subsymbols[subsymIdx].lutEntryIdx].value;
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
