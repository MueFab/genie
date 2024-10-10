/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
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
 * @brief
 */
class DecoderTokenType : public core::parameter::desc_pres::DecoderTokentype {
 protected:
    uint8_t rle_guard_tokentype : 8;                       //!< @brief
    std::vector<Subsequence> descriptor_subsequence_cfgs;  //!< @brief

 public:
    static const uint8_t MODE_CABAC = 0;  //!< @brief

    /**
     * @brief
     * @param dec
     * @return
     */
    bool equals(const Decoder *dec) const override {
        return core::parameter::desc_pres::Decoder::equals(dec) &&
               dynamic_cast<const DecoderTokenType *>(dec)->rle_guard_tokentype == rle_guard_tokentype &&
               dynamic_cast<const DecoderTokenType *>(dec)->rle_guard_tokentype == rle_guard_tokentype &&
               dynamic_cast<const DecoderTokenType *>(dec)->descriptor_subsequence_cfgs == descriptor_subsequence_cfgs;
    }

    /**
     * @brief
     */
    DecoderTokenType();

    /**
     * @brief
     * @param desc
     * @param reader
     */
    explicit DecoderTokenType(core::GenDesc desc, util::BitReader &reader);

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
    static std::unique_ptr<core::parameter::desc_pres::DecoderTokentype> create(genie::core::GenDesc desc,
                                                                                util::BitReader &reader);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getRleGuardTokentype() const;
};

/**
 * @brief
 */
class DecoderRegular : public core::parameter::desc_pres::DecoderRegular {
 protected:
    std::vector<Subsequence> descriptor_subsequence_cfgs;  //!< @brief

 public:
    static const uint8_t MODE_CABAC = 0;  //!< @brief

    /**
     * @brief
     * @param dec
     * @return
     */
    bool equals(const Decoder *dec) const override {
        return core::parameter::desc_pres::Decoder::equals(dec) &&
               dynamic_cast<const DecoderRegular *>(dec)->descriptor_subsequence_cfgs == descriptor_subsequence_cfgs;
    }

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

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
