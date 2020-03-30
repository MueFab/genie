/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_LUT_SUBSYMBOL_TRANSFORM_H_
#define GABAC_LUT_SUBSYMBOL_TRANSFORM_H_

#include "reader.h"
#include "writer.h"
#include <genie/entropy/paramcabac/state_vars.h>

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
namespace gabac {

struct LutEntry {
    uint64_t value;
    uint64_t index;
    uint64_t freq; // used only in encoder

    bool operator < (const LutEntry& entry) const
    {
        return (freq < entry.freq);
    }

    bool operator > (const LutEntry& entry) const
    {
        return (freq > entry.freq);
    }
};

struct LutRow {
    std::vector<LutEntry> entries;  // max size numAlphaSubsym
    uint64_t numMaxElems;           // max value == numAlphaSubsym
};

typedef std::vector<LutRow>    LutOrder1;
typedef std::vector<LutOrder1> LutOrder2;

class LUTsSubSymbolTransformation {

    public:
    LUTsSubSymbolTransformation(const paramcabac::SupportValues& _supportVals,
                                const paramcabac::StateVars& _stateVars,
                                const bool _modeFlag);

    void decodeLUTs(Reader &reader);
    void encodeLUTs(Writer &writer);

    void invTransform(util::DataBlock& symbolsIn, util::DataBlock& symbolsOut);
    void transform(util::DataBlock& symbolsIn, util::DataBlock& symbolsOut);

    private:
    void setupLutsO1(uint8_t numSubsyms, uint64_t numAlphaSubsym);
    void setupLutsO2(uint8_t numSubsyms, uint64_t numAlphaSubsym);

    void sortLutRow(LutRow& lutRow);
    void buildLuts(util::DataBlock& symbols);

    void decodeLutOrder1(Reader &reader, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut);
    void encodeLutOrder1(Writer &writer, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut);

    std::vector<LutOrder1> lutsO1;  // max size numSubsyms
    std::vector<LutOrder2> lutsO2;  // max size numSubsyms

    const paramcabac::SupportValues& supportVals;
    const paramcabac::StateVars& stateVars;
    bool encodingModeFlag;
};

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_LUT_SUBSYMBOL_TRANSFORM_H_
