/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CABAC_DECODER_CONF_H
#define GENIE_CABAC_DECODER_CONF_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/descriptor_present/decoder-regular.h>
#include <genie/core/parameter/descriptor_present/decoder-tokentype.h>
#include <genie/util/bitwriter.h>
#include "subsequence.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 *
 */
class DecoderTokenType : public core::parameter::desc_pres::DecoderTokentype {
   protected:
    uint8_t rle_guard_tokentype : 8;                       //!<
    std::vector<Subsequence> descriptor_subsequence_cfgs;  //!<
   public:
    static const uint8_t MODE_CABAC = 0;  //!<

    /**
     *
     */
    explicit DecoderTokenType();

    /**
     *
     * @param reader
     */
    explicit DecoderTokenType(util::BitReader &reader);

    /**
     *
     * @param index
     * @param cfg
     */
    void setSubsequenceCfg(uint8_t index, Subsequence &&cfg);

    /**
     *
     * @param index
     * @return
     */
    const Subsequence &getSubsequenceCfg(uint8_t index) const;

    /**
     *
     * @param index
     * @return
     */
    Subsequence &getSubsequenceCfg(uint8_t index);

    /**
     *
     * @return
     */
    std::unique_ptr<core::parameter::desc_pres::Decoder> clone() const override;

    /**
     *
     * @param reader
     * @return
     */
    static std::unique_ptr<core::parameter::desc_pres::DecoderTokentype> create(util::BitReader &reader);

    /**
     *
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     *
     * @return
     */
    uint8_t getRleGuardTokentype() const;
};

/**
 *
 */
class DecoderRegular : public core::parameter::desc_pres::DecoderRegular {
   protected:
    std::vector<Subsequence> descriptor_subsequence_cfgs;  //!<

   public:
    static const uint8_t MODE_CABAC = 0;  //!<

    /**
     *
     */
    DecoderRegular();

    /**
     *
     * @param desc
     */
    explicit DecoderRegular(core::GenDesc desc);

    /**
     *
     * @param reader
     */
    explicit DecoderRegular(util::BitReader &reader);

    /**
     *
     * @param index
     * @param cfg
     */
    void setSubsequenceCfg(uint8_t index, Subsequence &&cfg);

    /**
     *
     * @param index
     * @return
     */
    const Subsequence &getSubsequenceCfg(uint8_t index) const;

    /**
     *
     * @param index
     * @return
     */
    Subsequence &getSubsequenceCfg(uint8_t index);

    /**
     *
     * @return
     */
    std::unique_ptr<core::parameter::desc_pres::Decoder> clone() const override;

    /**
     *
     * @param reader
     * @return
     */
    static std::unique_ptr<core::parameter::desc_pres::DecoderRegular> create(util::BitReader &reader);

    /**
     *
     * @param writer
     */
    void write(util::BitWriter &writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------