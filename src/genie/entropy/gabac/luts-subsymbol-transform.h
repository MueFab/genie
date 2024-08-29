/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_LUTS_SUBSYMBOL_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_LUTS_SUBSYMBOL_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/entropy/gabac/reader.h"
#include "genie/entropy/gabac/subsymbol.h"
#include "genie/entropy/gabac/writer.h"
#include "genie/entropy/paramcabac/state_vars.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {
class DataBlock;
}

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
struct LutEntry {
    uint64_t value;  //!< @brief
    uint64_t freq;   //!< used only in encoder

    /**
     * @brief
     * @param entry
     * @return
     */
    bool operator>(const LutEntry& entry) const;
};

/**
 * @brief
 */
struct LutRow {
    std::vector<LutEntry> entries;  //!< max size numAlphaSubsym
    uint64_t numMaxElems;           //!< max value == numAlphaSubsym
};

typedef std::vector<LutRow> LutOrder1;
typedef std::vector<LutOrder1> LutOrder2;

/**
 * @brief
 */
class LUTsSubSymbolTransform {
 public:
    /**
     * @brief
     * @param _supportVals
     * @param _stateVars
     * @param _numLuts
     * @param _numPrvs
     * @param _modeFlag
     */
    LUTsSubSymbolTransform(const paramcabac::SupportValues& _supportVals, const paramcabac::StateVars& _stateVars,
                           uint8_t _numLuts, uint8_t _numPrvs, bool _modeFlag);

    /**
     * @brief
     * @param src
     */
    LUTsSubSymbolTransform(const LUTsSubSymbolTransform& src);

    /**
     * @brief
     * @param reader
     */
    void decodeLUTs(Reader& reader);

    /**
     * @brief
     * @param writer
     * @param symbols
     * @param depSymbols
     */
    void encodeLUTs(Writer& writer, util::DataBlock* symbols, util::DataBlock* depSymbols = nullptr);

    /**
     * @brief
     * @param subsymbols
     * @param lutIdx
     * @param prvIdx
     * @return
     */
    uint64_t getNumMaxElemsOrder2(std::vector<Subsymbol>& subsymbols, uint8_t lutIdx, uint8_t prvIdx);

    /**
     * @brief
     * @param subsymbols
     * @param lutIdx
     * @param prvIdx
     * @return
     */
    uint64_t getNumMaxElemsOrder1(std::vector<Subsymbol>& subsymbols, uint8_t lutIdx, uint8_t prvIdx);

    /**
     * @brief
     * @param subsymbols
     * @param subsymIdx
     * @param lutIdx
     * @param prvIdx
     */
    void invTransformOrder2(std::vector<Subsymbol>& subsymbols, uint8_t subsymIdx, uint8_t lutIdx, uint8_t prvIdx);

    /**
     * @brief
     * @param subsymbols
     * @param subsymIdx
     * @param lutIdx
     * @param prvIdx
     */
    void invTransformOrder1(std::vector<Subsymbol>& subsymbols, uint8_t subsymIdx, uint8_t lutIdx, uint8_t prvIdx);

    /**
     * @brief
     * @param subsymbols
     * @param subsymIdx
     * @param lutIdx
     * @param prvIdx
     */
    void transformOrder2(std::vector<Subsymbol>& subsymbols, uint8_t subsymIdx, uint8_t lutIdx, uint8_t prvIdx);

    /**
     * @brief
     * @param subsymbols
     * @param subsymIdx
     * @param lutIdx
     * @param prvIdx
     */
    void transformOrder1(std::vector<Subsymbol>& subsymbols, uint8_t subsymIdx, uint8_t lutIdx, uint8_t prvIdx);

 private:
    /**
     * @brief
     * @param numAlphaSubsym
     * @return
     */
    static inline LutOrder1 getInitLutsOrder1(uint64_t numAlphaSubsym);

    /**
     * @brief
     * @param numSubsyms
     * @param numAlphaSubsym
     */
    void setupLutsOrder1(uint8_t numSubsyms, uint64_t numAlphaSubsym);

    /**
     * @brief
     * @param numSubsyms
     * @param numAlphaSubsym
     */
    void setupLutsOrder2(uint8_t numSubsyms, uint64_t numAlphaSubsym);

    /**
     * @brief
     * @param symbols
     * @param depSymbols
     */
    void buildLuts(util::DataBlock* symbols, util::DataBlock* depSymbols = nullptr);

    /**
     * @brief
     * @param lutRow
     */
    static void sortLutRow(LutRow& lutRow);

    /**
     * @brief
     * @param reader
     * @param numAlphaSubsym
     * @param codingSubsymSize
     * @param lut
     */
    static void decodeLutOrder1(Reader& reader, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut);

    /**
     * @brief
     * @param writer
     * @param numAlphaSubsym
     * @param codingSubsymSize
     * @param lut
     */
    void encodeLutOrder1(Writer& writer, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut);

    const paramcabac::SupportValues supportVals;  //!< @brief
    const paramcabac::StateVars stateVars;        //!< @brief
    uint8_t numLuts;                              //!< @brief
    uint8_t numPrvs;                              //!< @brief
    bool encodingModeFlag;                        //!< @brief

    std::vector<LutOrder1> lutsO1;  //!< @brief max size numSubsyms
    std::vector<LutOrder2> lutsO2;  //!< @brief max size numSubsyms
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_LUTS_SUBSYMBOL_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
