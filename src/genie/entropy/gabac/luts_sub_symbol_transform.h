/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the LUT-based subsymbol transformation class and
 * associated structures.
 *
 * This file contains the declaration of the LUTsSubSymbolTransform class, which
 * implements a series of transformations using Look-Up Tables (LUTs) for
 * subsymbol encoding and decoding. It includes support for both order-1 and
 * order-2 LUTs, allowing for various types of transformation configurations.
 *
 * @details The LUT-based subsymbol transformations are used to encode and
 * Decode sequences of genomic data. They can transform symbols using a
 * specified configuration, reducing the redundancy in data representation. This
 * file also defines structures to represent LUT entries and rows, and provides
 * utility functions for encoding, decoding, and building LUTs.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_LUTS_SUB_SYMBOL_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_LUTS_SUB_SYMBOL_TRANSFORM_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/entropy/gabac/reader.h"
#include "genie/entropy/gabac/sub_symbol.h"
#include "genie/entropy/gabac/writer.h"
#include "genie/entropy/paramcabac/state_vars.h"

// -----------------------------------------------------------------------------

namespace genie::util {
class DataBlock;
}

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Represents an entry in a LUT row.
 *
 * This structure contains a value and frequency for a given LUT entry. The
 * frequency is used primarily in encoding to determine the most frequently
 * occurring values.
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
 * A LUT row contains multiple LUT entries and keeps track of the maximum number
 * of elements. It is used to store and manage mappings of values for subsymbol
 * transformations.
 */
struct LutRow {
  std::vector<LutEntry> entries;  //!< List of entries in this LUT row.
  uint64_t num_max_elems;  //!< The maximum number of elements for this LUT row.
};

typedef std::vector<LutRow>
    LutOrder1;  //!< LUT structure for order-1 transformations.
typedef std::vector<LutOrder1>
    LutOrder2;  //!< LUT structure for order-2 transformations.

/**
 * @brief Class to manage LUT-based transformations for subsymbol encoding and
 * decoding.
 *
 * This class implements transformations using both order-1 and order-2 LUTs. It
 * provides methods for building, encoding, decoding, and transforming subsymbol
 * data using the configured LUTs.
 */
class LuTsSubSymbolTransform {
 public:
  /**
   * @brief Constructs an LUTsSubSymbolTransform object with the specified
   * configurations.
   *
   * @param support_vals The support values to configure the transformation.
   * @param state_vars The state variables to configure the transformation.
   * @param num_luts Number of LUTs used in the transformation.
   * @param num_prvs Number of previous symbols considered for the
   * transformation.
   * @param mode_flag Flag to indicate the encoding mode.
   */
  LuTsSubSymbolTransform(const paramcabac::SupportValues& support_vals,
                         const paramcabac::StateVars& state_vars,
                         uint8_t num_luts, uint8_t num_prvs, bool mode_flag);

  /**
   * @brief Copy constructor for LUTsSubSymbolTransform.
   *
   * Creates a copy of the provided LUTsSubSymbolTransform object.
   *
   * @param src The source LUTsSubSymbolTransform to copy from.
   */
  LuTsSubSymbolTransform(const LuTsSubSymbolTransform& src);

  /**
   * @brief Decodes LUTs from the provided Reader object.
   * @param reader The Reader object to Decode the LUTs from.
   */
  void DecodeLuTs(Reader& reader);

  /**
   * @brief Encodes LUTs to the provided Writer object using the given
   * symbols.
   * @param writer The Writer object to encode the LUTs to.
   * @param symbols Pointer to the DataBlock containing symbols for encoding.
   * @param dep_symbols Pointer to the DataBlock containing dependent symbols
   * (optional).
   */
  void EncodeLuTs(Writer& writer, const util::DataBlock* symbols,
                  const util::DataBlock* dep_symbols = nullptr);

  /**
   * @brief Retrieves the maximum number of elements for order-2 LUTs.
   * @param subsymbols Vector of subsymbols to analyze.
   * @param lut_idx Index of the LUT.
   * @param prv_idx Index of the previous symbol.
   * @return The maximum number of elements in the order-2 LUT.
   */
  [[nodiscard]] uint64_t GetNumMaxElemsOrder2(
      const std::vector<Subsymbol>& subsymbols, uint8_t lut_idx,
      uint8_t prv_idx) const;

  /**
   * @brief Retrieves the maximum number of elements for order-1 LUTs.
   * @param subsymbols Vector of subsymbols to analyze.
   * @param lut_idx Index of the LUT.
   * @param prv_idx Index of the previous symbol.
   * @return The maximum number of elements in the order-1 LUT.
   */
  [[nodiscard]] uint64_t GetNumMaxElemsOrder1(
      const std::vector<Subsymbol>& subsymbols, uint8_t lut_idx,
      uint8_t prv_idx) const;

  // Transformation methods (encode/Decode subsymbols)
  void InvTransformOrder2(std::vector<Subsymbol>& subsymbols,
                          uint8_t subsym_idx, uint8_t lut_idx,
                          uint8_t prv_idx) const;
  void InvTransformOrder1(std::vector<Subsymbol>& subsymbols,
                          uint8_t subsym_idx, uint8_t lut_idx,
                          uint8_t prv_idx) const;
  void TransformOrder2(std::vector<Subsymbol>& subsymbols, uint8_t subsym_idx,
                       uint8_t lut_idx, uint8_t prv_idx) const;
  void TransformOrder1(std::vector<Subsymbol>& subsymbols, uint8_t subsym_idx,
                       uint8_t lut_idx, uint8_t prv_idx) const;

 private:
  // Internal methods and variables for managing LUTs.
  static inline LutOrder1 GetInitLutsOrder1(uint64_t num_alpha_subsym);
  void SetupLutsOrder1(uint8_t num_subsyms, uint64_t num_alpha_subsym);
  void SetupLutsOrder2(uint8_t num_subsyms, uint64_t num_alpha_subsym);
  void BuildLuts(const util::DataBlock* symbols,
                 const util::DataBlock* dep_symbols = nullptr);
  static void SortLutRow(LutRow& lut_row);
  static void DecodeLutOrder1(Reader& reader, uint64_t num_alpha_subsym,
                              uint8_t coding_subsym_size, LutOrder1& lut);
  static void EncodeLutOrder1(Writer& writer, uint64_t num_alpha_subsym,
                              uint8_t coding_subsym_size, LutOrder1& lut);

  // Configuration and state information for LUT transformations.
  const paramcabac::SupportValues
      support_vals_;  //!< Support values for the transformation.
  const paramcabac::StateVars
      state_vars_;           //!< State variables for the transformation.
  uint8_t num_luts_;         //!< Number of LUTs used.
  uint8_t num_prvs_;         //!< Number of previous symbols considered.
  bool encoding_mode_flag_;  //!< Flag to indicate encoding or decoding mode.

  std::vector<LutOrder1> luts_o_1_;  //!< Order-1 LUTs.
  std::vector<LutOrder2> luts_o_2_;  //!< Order-2 LUTs.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_LUTS_SUB_SYMBOL_TRANSFORM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------