/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the Quantizer class for mapping quality values.
 *
 * This file defines the `Quantizer` class, which maps genomic quality values to
 * compact index representations using a look-up table (LUT). The class provides
 * methods to transform quality values to their respective indices and
 * reconstruction values for efficient encoding and decoding of quality scores.
 *
 * @details The `Quantizer` class allows for efficient representation of quality values
 * by quantizing them into a set of indices and corresponding reconstruction values.
 * It maintains a forward and inverse LUT for encoding and decoding operations. The LUT
 * maps quality values to indices, and indices to reconstruction values, facilitating
 * compact storage and quick lookup during the quantization process.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_QUANTIZER_H_
#define SRC_GENIE_QUALITY_CALQ_QUANTIZER_H_

#include <map>
#include <utility>

namespace genie::quality::calq {

/**
 * @brief Class for quantizing genomic quality values.
 *
 * The `Quantizer` class provides functionality to map quality values into indices
 * and reconstruction values using a forward and inverse lookup table (LUT).
 * It is used to convert the high-resolution quality values into a more compact
 * representation that can be easily stored and transmitted.
 *
 * @details The quantizer class is equipped with methods for bidirectional mapping
 * between quality values and their indices, as well as reconstruction values.
 * The LUTs are constructed based on the mapping specifications provided during
 * initialization. The class can print the LUTs for debugging purposes and supports
 * various mapping and lookup functions.
 */
class Quantizer {
 public:
    /**
     * @brief Default constructor for the `Quantizer` class.
     *
     * Initializes an empty `Quantizer` with no predefined mapping. The LUTs need to
     * be populated manually or through the specialized constructor.
     */
    Quantizer();

    /**
     * @brief Specialized constructor with an inverse LUT.
     *
     * Initializes the quantizer with a predefined inverse LUT. The `inverseLut`
     * parameter maps indices to reconstruction values, and the forward LUT is
     * generated automatically based on this mapping.
     *
     * @param inverseLut The inverse LUT mapping indices to reconstruction values.
     */
    explicit Quantizer(const std::map<int, int>& inverseLut);

    /**
     * @brief Destructor for the `Quantizer` class.
     *
     * Cleans up the resources and internal mappings of the quantizer.
     */
    virtual ~Quantizer();

    /**
     * @brief Maps a quality value to its index.
     *
     * This method uses the forward LUT to map a given quality value to its respective
     * index. The index is a more compact representation of the quality value used
     * for encoding.
     *
     * @param value The quality value to be mapped.
     * @return The index corresponding to the given quality value.
     */
    [[nodiscard]] int valueToIndex(const int& value) const;

    /**
     * @brief Maps an index to its reconstruction value.
     *
     * This method uses the inverse LUT to map a given index to its reconstruction
     * value. The reconstruction value is used to approximate the original quality value.
     *
     * @param index The index to be mapped.
     * @return The reconstruction value corresponding to the given index.
     */
    [[nodiscard]] int indexToReconstructionValue(const int& index) const;

    /**
     * @brief Maps a quality value to its reconstruction value.
     *
     * This method combines the forward and inverse LUTs to directly map a quality
     * value to its corresponding reconstruction value.
     *
     * @param value The quality value to be mapped.
     * @return The reconstruction value for the given quality value.
     */
    [[nodiscard]] int valueToReconstructionValue(const int& value) const;

    /**
     * @brief Retrieves the inverse LUT of the quantizer.
     *
     * Provides read-only access to the inverse LUT, which maps indices to
     * reconstruction values.
     *
     * @return A constant reference to the inverse LUT of the quantizer.
     */
    [[nodiscard]] const std::map<int, int>& inverseLut() const;

    /**
     * @brief Prints the content of the LUTs for debugging purposes.
     *
     * Outputs the forward and inverse LUTs to the console in a human-readable
     * format. This method is mainly used for debugging and verification of
     * the LUT mappings.
     */
    void print() const;

 protected:
    std::map<int, std::pair<int, int>> lut_;  //!< @brief Maps quality values to (index, reconstruction value) pairs.
    std::map<int, int> inverseLut_;           //!< @brief Maps indices to reconstruction values.
};

}  // namespace genie::quality::calq

#endif  // SRC_GENIE_QUALITY_CALQ_QUANTIZER_H_
