/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_LUT_SUBSYMBOL_TRANSFORM_H_
#define GABAC_LUT_SUBSYMBOL_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/entropy/paramcabac/state_vars.h>
#include "reader.h"
#include "writer.h"
#include "subsymbol.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
struct LutEntry {
    uint64_t value;  //!<
    uint64_t freq;   //!< used only in encoder

    /**
     *
     * @param entry
     * @return
     */
    bool operator>=(const LutEntry& entry) const;
};

/**
 *
 */
struct LutRow {
    std::vector<LutEntry> entries;  //!< max size numAlphaSubsym
    uint64_t numMaxElems;           //!< max value == numAlphaSubsym
};

typedef std::vector<LutRow> LutOrder1;
typedef std::vector<LutOrder1> LutOrder2;

/**
 *
 */
class LUTsSubSymbolTransform {
   public:
    /**
     *
     * @param _supportVals
     * @param _stateVars
     * @param _numLuts
     * @param _numPrvs
     * @param _modeFlag
     */
    LUTsSubSymbolTransform(const paramcabac::SupportValues& _supportVals, const paramcabac::StateVars& _stateVars,
                           const uint8_t _numLuts, const uint8_t _numPrvs, const bool _modeFlag);

    /**
     *
     * @param reader
     */
    void decodeLUTs(Reader& reader);

    /**
     *
     * @param writer
     * @param symbols
     * @param depSymbols
     */
    void encodeLUTs(Writer& writer, util::DataBlock* const symbols,
                    util::DataBlock* const depSymbols = nullptr);

    /**
     *
     * @param subsymbols
     * @param lutIdx
     * @param prvIdx
     * @return
     */
    uint64_t getNumMaxElemsOrder2(std::vector<Subsymbol>& subsymbols, const uint8_t lutIdx, const uint8_t prvIdx);

    /**
     *
     * @param subsymbols
     * @param lutIdx
     * @param prvIdx
     * @return
     */
    uint64_t getNumMaxElemsOrder1(std::vector<Subsymbol>& subsymbols, const uint8_t lutIdx, const uint8_t prvIdx);

    /**
     *
     * @param subsymbols
     * @param subsymIdx
     * @param lutIdx
     * @param prvIdx
     */
    void invTransformOrder2(std::vector<Subsymbol>& subsymbols, const uint8_t subsymIdx, const uint8_t lutIdx,
                            const uint8_t prvIdx);

    /**
     *
     * @param subsymbols
     * @param subsymIdx
     * @param lutIdx
     * @param prvIdx
     */
    void invTransformOrder1(std::vector<Subsymbol>& subsymbols, const uint8_t subsymIdx, const uint8_t lutIdx,
                            const uint8_t prvIdx);

    /**
     *
     * @param subsymbols
     * @param subsymIdx
     * @param lutIdx
     * @param prvIdx
     */
    void transformOrder2(std::vector<Subsymbol>& subsymbols, const uint8_t subsymIdx, const uint8_t lutIdx,
                         const uint8_t prvIdx);

    /**
     *
     * @param subsymbols
     * @param subsymIdx
     * @param lutIdx
     * @param prvIdx
     */
    void transformOrder1(std::vector<Subsymbol>& subsymbols, const uint8_t subsymIdx, const uint8_t lutIdx,
                         const uint8_t prvIdx);

   private:
    /**
     *
     * @param numAlphaSubsym
     * @return
     */
    inline LutOrder1 getInitLutsOrder1(uint64_t numAlphaSubsym);

    /**
     *
     * @param numSubsyms
     * @param numAlphaSubsym
     */
    void setupLutsOrder1(uint8_t numSubsyms, uint64_t numAlphaSubsym);

    /**
     *
     * @param numSubsyms
     * @param numAlphaSubsym
     */
    void setupLutsOrder2(uint8_t numSubsyms, uint64_t numAlphaSubsym);

    /**
     *
     * @param symbols
     * @param depSymbols
     */
    void buildLuts(util::DataBlock* const symbols, util::DataBlock* const depSymbols = nullptr);

    /**
     *
     * @param lutRow
     */
    void sortLutRow(LutRow& lutRow);

    /**
     *
     * @param reader
     * @param numAlphaSubsym
     * @param codingSubsymSize
     * @param lut
     */
    void decodeLutOrder1(Reader& reader, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut);

    /**
     *
     * @param writer
     * @param numAlphaSubsym
     * @param codingSubsymSize
     * @param lut
     */
    void encodeLutOrder1(Writer& writer, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut);

    std::vector<LutOrder1> lutsO1;  //!< max size numSubsyms
    std::vector<LutOrder2> lutsO2;  //!< max size numSubsyms

    const paramcabac::SupportValues& supportVals;  //!<
    const paramcabac::StateVars& stateVars;        //!<
    uint8_t numLuts;                               //!<
    uint8_t numPrvs;                               //!<
    bool encodingModeFlag;                         //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_LUT_SUBSYMBOL_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------