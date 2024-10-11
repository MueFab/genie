/**
 * @file
 * @brief Header file for the LZMA-specific decoder configuration class.
 * @details This file contains the definition of the `DecoderRegular` class, which represents
 *          a descriptor parameter configuration for an LZMA-based decoder. The `DecoderRegular`
 *          class implements the necessary interface to read, configure, and write LZMA decoder settings
 *          used within the GENIE framework.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_LZMA_PARAM_DECODER_H_
#define SRC_GENIE_ENTROPY_LZMA_PARAM_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/entropy/lzma/subsequence.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::lzma {

/**
 * @brief LZMA-specific parameter configuration for the decoder.
 * @details The `DecoderRegular` class is derived from `core::parameter::desc_pres::DecoderRegular`
 *          and provides LZMA-specific configurations for decoding descriptor subsequences. It stores
 *          configurations for each subsequence and offers methods for setting, getting, and serializing
 *          these parameters.
 */
class DecoderRegular : public core::parameter::desc_pres::DecoderRegular {
 protected:
    std::vector<Subsequence> descriptor_subsequence_cfgs;  //!< @brief List of subsequence configurations.

 public:
    static const uint8_t MODE_LZMA = 1;  //!< @brief LZMA mode identifier.

    /**
     * @brief Compare two decoder configurations.
     * @param dec Pointer to the decoder to compare.
     * @return True if the configurations are equal.
     */
    bool equals(const Decoder *dec) const override;

    /**
     * @brief Default constructor for an empty DecoderRegular object.
     */
    DecoderRegular();

    /**
     * @brief Construct a DecoderRegular object with a specified descriptor.
     * @param desc Descriptor to associate with this decoder.
     */
    explicit DecoderRegular(core::GenDesc desc);

    /**
     * @brief Construct a DecoderRegular object from a BitReader.
     * @param desc Descriptor to associate with this decoder.
     * @param reader Reference to the BitReader used to extract the configuration.
     */
    explicit DecoderRegular(core::GenDesc desc, util::BitReader &reader);

    /**
     * @brief Set the subsequence configuration at the given index.
     * @param index Index of the subsequence configuration.
     * @param cfg The new subsequence configuration to be set.
     */
    void setSubsequenceCfg(uint8_t index, Subsequence &&cfg);

    /**
     * @brief Get the subsequence configuration at the specified index.
     * @param index Index of the configuration to retrieve.
     * @return Const reference to the subsequence configuration at the specified index.
     */
    [[nodiscard]] const Subsequence &getSubsequenceCfg(uint8_t index) const;

    /**
     * @brief Get the subsequence configuration at the specified index.
     * @param index Index of the configuration to retrieve.
     * @return Modifiable reference to the subsequence configuration at the specified index.
     */
    Subsequence &getSubsequenceCfg(uint8_t index);

    /**
     * @brief Create a copy of the current decoder configuration.
     * @return A unique pointer to the new cloned `DecoderRegular` object.
     */
    [[nodiscard]] std::unique_ptr<core::parameter::desc_pres::Decoder> clone() const override;

    /**
     * @brief Create a new `DecoderRegular` object from a BitReader.
     * @param desc Descriptor to associate with the decoder.
     * @param reader Reference to the BitReader to extract the configuration.
     * @return A unique pointer to the newly created `DecoderRegular` object.
     */
    static std::unique_ptr<core::parameter::desc_pres::DecoderRegular> create(genie::core::GenDesc desc,
                                                                              util::BitReader &reader);

    /**
     * @brief Serialize the configuration and write it to a BitWriter.
     * @param writer Reference to the BitWriter to store the serialized configuration.
     */
    void write(util::BitWriter &writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_LZMA_PARAM_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
