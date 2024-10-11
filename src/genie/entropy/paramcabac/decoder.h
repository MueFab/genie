/**
 * @file
 * @brief Defines classes for CABAC (Context-based Adaptive Binary Arithmetic Coding) decoders: `DecoderTokenType` and
 *        `DecoderRegular`. These decoders handle specific CABAC configurations for different descriptor types in GENIE.
 * @details The file provides implementations of CABAC decoder classes derived from the base GENIE decoders.
 *          `DecoderTokenType` and `DecoderRegular` are used for handling different CABAC transformations in the
 *          entropy decoding process. Each class manages a collection of subsequences, which define the transformation
 *          parameters and structures for entropy coding. The classes include serialization and deserialization
 *          capabilities for the defined parameters and transformations.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_DECODER_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/core/parameter/descriptor_present/decoder-tokentype.h"
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief CABAC decoder for handling token type transformations.
 * @details This decoder manages token type-based subsequence transformations and configurations used during CABAC
 *          entropy decoding. It supports multiple subsequences and provides methods for reading and writing these
 *          configurations.
 */
class DecoderTokenType : public core::parameter::desc_pres::DecoderTokentype {
 protected:
    uint8_t rle_guard_tokentype : 8;                       //!< @brief RLE (Run-Length Encoding) guard for token types.
    std::vector<Subsequence> descriptor_subsequence_cfgs;  //!< @brief Subsequence configurations for the decoder.

 public:
    static const uint8_t MODE_CABAC = 0;  //!< @brief Constant representing CABAC mode.

    /**
     * @brief Default constructor.
     */
    DecoderTokenType();

    /**
     * @brief Constructs a `DecoderTokenType` object from a bitstream reader.
     * @param desc Descriptor ID for the token type decoder.
     * @param reader Bitstream reader object.
     */
    explicit DecoderTokenType(core::GenDesc desc, util::BitReader &reader);

    /**
     * @brief Sets a subsequence configuration at a given index.
     * @param index Index of the subsequence configuration.
     * @param cfg Subsequence configuration to set.
     */
    void setSubsequenceCfg(uint8_t index, Subsequence &&cfg);

    /**
     * @brief Retrieves a constant reference to a subsequence configuration at a given index.
     * @param index Index of the subsequence configuration.
     * @return Constant reference to the subsequence configuration.
     */
    [[nodiscard]] const Subsequence &getSubsequenceCfg(uint8_t index) const;

    /**
     * @brief Retrieves a reference to a subsequence configuration at a given index.
     * @param index Index of the subsequence configuration.
     * @return Reference to the subsequence configuration.
     */
    Subsequence &getSubsequenceCfg(uint8_t index);

    /**
     * @brief Creates a clone of the current decoder.
     * @return A unique pointer to the cloned `DecoderTokenType`.
     */
    [[nodiscard]] std::unique_ptr<core::parameter::desc_pres::Decoder> clone() const override;

    /**
     * @brief Creates a `DecoderTokenType` from a bitstream reader.
     * @param desc Descriptor ID for the decoder.
     * @param reader Bitstream reader object.
     * @return A unique pointer to the created `DecoderTokenType`.
     */
    static std::unique_ptr<core::parameter::desc_pres::DecoderTokentype> create(genie::core::GenDesc desc,
                                                                                util::BitReader &reader);

    /**
     * @brief Writes the `DecoderTokenType` to a bitstream.
     * @param writer Bitstream writer object.
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief Retrieves the RLE guard for token types.
     * @return The RLE guard for token types.
     */
    [[nodiscard]] uint8_t getRleGuardTokentype() const;

    /**
     * @brief Compares two decoders for equality.
     * @param dec Pointer to the other decoder object.
     * @return True if both decoders are equal, false otherwise.
     */
    bool equals(const Decoder *dec) const override;
};

/**
 * @brief CABAC decoder for handling regular transformations.
 * @details This decoder manages regular subsequence transformations and configurations used during CABAC entropy
 *          decoding. It supports multiple subsequences and provides methods for reading and writing these configurations.
 */
class DecoderRegular : public core::parameter::desc_pres::DecoderRegular {
 protected:
    std::vector<Subsequence> descriptor_subsequence_cfgs;  //!< @brief Subsequence configurations for the decoder.

 public:
    static const uint8_t MODE_CABAC = 0;  //!< @brief Constant representing CABAC mode.

    /**
     * @brief Default constructor.
     */
    DecoderRegular();

    /**
     * @brief Constructs a `DecoderRegular` object from a descriptor ID.
     * @param desc Descriptor ID for the regular decoder.
     */
    explicit DecoderRegular(core::GenDesc desc);

    /**
     * @brief Constructs a `DecoderRegular` object from a bitstream reader.
     * @param desc Descriptor ID for the regular decoder.
     * @param reader Bitstream reader object.
     */
    explicit DecoderRegular(core::GenDesc desc, util::BitReader &reader);

    /**
     * @brief Sets a subsequence configuration at a given index.
     * @param index Index of the subsequence configuration.
     * @param cfg Subsequence configuration to set.
     */
    void setSubsequenceCfg(uint8_t index, Subsequence &&cfg);

    /**
     * @brief Retrieves a constant reference to a subsequence configuration at a given index.
     * @param index Index of the subsequence configuration.
     * @return Constant reference to the subsequence configuration.
     */
    [[nodiscard]] const Subsequence &getSubsequenceCfg(uint8_t index) const;

    /**
     * @brief Retrieves a reference to a subsequence configuration at a given index.
     * @param index Index of the subsequence configuration.
     * @return Reference to the subsequence configuration.
     */
    Subsequence &getSubsequenceCfg(uint8_t index);

    /**
     * @brief Creates a clone of the current decoder.
     * @return A unique pointer to the cloned `DecoderRegular`.
     */
    [[nodiscard]] std::unique_ptr<core::parameter::desc_pres::Decoder> clone() const override;

    /**
     * @brief Creates a `DecoderRegular` from a bitstream reader.
     * @param desc Descriptor ID for the decoder.
     * @param reader Bitstream reader object.
     * @return A unique pointer to the created `DecoderRegular`.
     */
    static std::unique_ptr<core::parameter::desc_pres::DecoderRegular> create(genie::core::GenDesc desc,
                                                                              util::BitReader &reader);

    /**
     * @brief Writes the `DecoderRegular` to a bitstream.
     * @param writer Bitstream writer object.
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief Compares two decoders for equality.
     * @param dec Pointer to the other decoder object.
     * @return True if both decoders are equal, false otherwise.
     */
    bool equals(const Decoder *dec) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
