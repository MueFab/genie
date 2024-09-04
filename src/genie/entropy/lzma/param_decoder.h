/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
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
 *
 */
class DecoderRegular : public core::parameter::desc_pres::DecoderRegular {
 protected:
    std::vector<Subsequence> descriptor_subsequence_cfgs;  //!< @brief

 public:
    static const uint8_t MODE_LZMA = 1;  //!< @brief

    /**
     * @brief
     * @param dec
     * @return
     */
    bool equals(const Decoder *dec) const override;

    /**
     * @brief
     */
    DecoderRegular();

    /**
     * @brief
     * @param desc
     */
    explicit DecoderRegular(core::GenDesc desc);

    /**
     * @brief
     * @param desc
     * @param reader
     */
    explicit DecoderRegular(core::GenDesc desc, util::BitReader &reader);

    /**
     * @brief
     * @param index
     * @param cfg
     */
    void setSubsequenceCfg(uint8_t index, Subsequence &&cfg);

    /**
     * @brief
     * @param index
     * @return
     */
    [[nodiscard]] const Subsequence &getSubsequenceCfg(uint8_t index) const;

    /**
     * @brief
     * @param index
     * @return
     */
    Subsequence &getSubsequenceCfg(uint8_t index);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::unique_ptr<core::parameter::desc_pres::Decoder> clone() const override;

    /**
     * @brief
     * @param desc
     * @param reader
     * @return
     */
    static std::unique_ptr<core::parameter::desc_pres::DecoderRegular> create(genie::core::GenDesc desc,
                                                                              util::BitReader &reader);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_LZMA_PARAM_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
