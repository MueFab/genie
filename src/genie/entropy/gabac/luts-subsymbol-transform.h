/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the LUT-based subsymbol transformation class and associated structures.
 *
 * This file contains the declaration of the LUTsSubSymbolTransform class, which implements a series of transformations
 * using Look-Up Tables (LUTs) for subsymbol encoding and decoding. It includes support for both order-1 and order-2 LUTs,
 * allowing for various types of transformation configurations.
 *
 * @details The LUT-based subsymbol transformations are used to encode and decode sequences of genomic data. They can
 * transform symbols using a specified configuration, reducing the redundancy in data representation. This file also
 * defines structures to represent LUT entries and rows, and provides utility functions for encoding, decoding, and
 * building LUTs.
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
 * @brief Represents an entry in a LUT row.
 *
 * This structure contains a value and frequency for a given LUT entry. The frequency is used
 * primarily in encoding to determine the most frequently occurring values.
 */
struct LutEntry {
    uint64_t value;  //!< The value of the LUT entry.
    uint64_t freq;   //!< The frequency of this value (used in the encoder).

    /**
     * @brief Compares two LUT entries based on their frequency.
     * @param entry The LUT entry to compare against.
     * @return True if this entry has a higher frequency than the other entry.
     */
    bool operator>(const LutEntry& entry) const;
};

/**
 * @brief Represents a row in a LUT.
 *
 * A LUT row contains multiple LUT entries and keeps track of the maximum number of elements.
 * It is used to store and manage mappings of values for subsymbol transformations.
 */
struct LutRow {
    std::vector<LutEntry> entries;  //!< List of entries in this LUT row.
    uint64_t numMaxElems;           //!< The maximum number of elements for this LUT row.
};

typedef std::vector<LutRow> LutOrder1;  //!< LUT structure for order-1 transformations.
typedef std::vector<LutOrder1> LutOrder2;  //!< LUT structure for order-2 transformations.

/**
 * @brief Class to manage LUT-based transformations for subsymbol encoding and decoding.
 *
 * This class implements transformations using both order-1 and order-2 LUTs. It provides methods for
 * building, encoding, decoding, and transforming subsymbol data using the configured LUTs.
 */
class LUTsSubSymbolTransform {
 public:
    /**
     * @brief Constructs an LUTsSubSymbolTransform object with the specified configurations.
     *
     * @param _supportVals The support values to configure the transformation.
     * @param _stateVars The state variables to configure the transformation.
     * @param _numLuts Number of LUTs used in the transformation.
     * @param _numPrvs Number of previous symbols considered for the transformation.
     * @param _modeFlag Flag to indicate the encoding mode.
     */
    LUTsSubSymbolTransform(const paramcabac::SupportValues& _supportVals, const paramcabac::StateVars& _stateVars,
                           uint8_t _numLuts, uint8_t _numPrvs, bool _modeFlag);

    /**
     * @brief Copy constructor for LUTsSubSymbolTransform.
     *
     * Creates a copy of the provided LUTsSubSymbolTransform object.
     *
     * @param src The source LUTsSubSymbolTransform to copy from.
     */
    LUTsSubSymbolTransform(const LUTsSubSymbolTransform& src);

    /**
     * @brief Decodes LUTs from the provided Reader object.
     * @param reader The Reader object to decode the LUTs from.
     */
    void decodeLUTs(Reader& reader);

    /**
     * @brief Encodes LUTs to the provided Writer object using the given symbols.
     * @param writer The Writer object to encode the LUTs to.
     * @param symbols Pointer to the DataBlock containing symbols for encoding.
     * @param depSymbols Pointer to the DataBlock containing dependent symbols (optional).
     */
    void encodeLUTs(Writer& writer, util::DataBlock* symbols, util::DataBlock* depSymbols = nullptr);

    /**
     * @brief Retrieves the maximum number of elements for order-2 LUTs.
     * @param subsymbols Vector of subsymbols to analyze.
     * @param lutIdx Index of the LUT.
     * @param prvIdx Index of the previous symbol.
     * @return The maximum number of elements in the order-2 LUT.
     */
    uint64_t getNumMaxElemsOrder2(std::vector<Subsymbol>& subsymbols, uint8_t lutIdx, uint8_t prvIdx);

    /**
     * @brief Retrieves the maximum number of elements for order-1 LUTs.
     * @param subsymbols Vector of subsymbols to analyze.
     * @param lutIdx Index of the LUT.
     * @param prvIdx Index of the previous symbol.
     * @return The maximum number of elements in the order-1 LUT.
     */
    uint64_t getNumMaxElemsOrder1(std::vector<Subsymbol>& subsymbols, uint8_t lutIdx, uint8_t prvIdx);

    // Transformation methods (encode/decode subsymbols)
    void invTransformOrder2(std::vector<Subsymbol>& subsymbols, uint8_t subsymIdx, uint8_t lutIdx, uint8_t prvIdx);
    void invTransformOrder1(std::vector<Subsymbol>& subsymbols, uint8_t subsymIdx, uint8_t lutIdx, uint8_t prvIdx);
    void transformOrder2(std::vector<Subsymbol>& subsymbols, uint8_t subsymIdx, uint8_t lutIdx, uint8_t prvIdx);
    void transformOrder1(std::vector<Subsymbol>& subsymbols, uint8_t subsymIdx, uint8_t lutIdx, uint8_t prvIdx);

 private:
    // Internal methods and variables for managing LUTs.
    static inline LutOrder1 getInitLutsOrder1(uint64_t numAlphaSubsym);
    void setupLutsOrder1(uint8_t numSubsyms, uint64_t numAlphaSubsym);
    void setupLutsOrder2(uint8_t numSubsyms, uint64_t numAlphaSubsym);
    void buildLuts(util::DataBlock* symbols, util::DataBlock* depSymbols = nullptr);
    static void sortLutRow(LutRow& lutRow);
    static void decodeLutOrder1(Reader& reader, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut);
    static void encodeLutOrder1(Writer& writer, uint64_t numAlphaSubsym, uint8_t codingSubsymSize, LutOrder1& lut);

    // Configuration and state information for LUT transformations.
    const paramcabac::SupportValues supportVals;  //!< Support values for the transformation.
    const paramcabac::StateVars stateVars;        //!< State variables for the transformation.
    uint8_t numLuts;                              //!< Number of LUTs used.
    uint8_t numPrvs;                              //!< Number of previous symbols considered.
    bool encodingModeFlag;                        //!< Flag to indicate encoding or decoding mode.

    std::vector<LutOrder1> lutsO1;  //!< Order-1 LUTs.
    std::vector<LutOrder2> lutsO2;  //!< Order-2 LUTs.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_LUTS_SUBSYMBOL_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
