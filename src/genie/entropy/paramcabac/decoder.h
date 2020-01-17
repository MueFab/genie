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
 * Base for ISO 23092-2 Section 8.3.1 and ISO 23092-2 Section 8.3.5
 */
class DecoderTokenType : public core::parameter::desc_pres::DecoderTokentype {
   protected:
    uint8_t rle_guard_tokentype : 8;                                        //!< line 4
    std::vector<std::unique_ptr<Subsequence>> descriptor_subsequence_cfgs;  //!< Line 4 to 13
   public:
    static const uint8_t MODE_CABAC = 0;

    explicit DecoderTokenType();

    explicit DecoderTokenType(util::BitReader &reader);

    void setSubsequenceCfg(uint8_t index, std::unique_ptr<TransformedParameters> cfg);

    Subsequence *getSubsequenceCfg(uint8_t index) const;

    DecoderTokenType(const DecoderTokenType &c);

    std::unique_ptr<core::parameter::desc_pres::Decoder> clone() const override;

    static std::unique_ptr<core::parameter::desc_pres::DecoderTokentype> create(util::BitReader& reader) {
        return util::make_unique<DecoderTokenType>(reader);
    }

    void write(util::BitWriter &writer) const override;

    uint8_t getRleGuardTokentype() const;
};





/**
 * Base for ISO 23092-2 Section 8.3.1 and ISO 23092-2 Section 8.3.5
 */
class DecoderRegular : public core::parameter::desc_pres::DecoderRegular {
   protected:
    std::vector<std::unique_ptr<Subsequence>> descriptor_subsequence_cfgs;  //!< Line 4 to 13

   public:
    static const uint8_t MODE_CABAC = 0;

    DecoderRegular() : core::parameter::desc_pres::DecoderRegular(MODE_CABAC){

    }

    explicit DecoderRegular(core::GenDesc desc);

    explicit DecoderRegular(util::BitReader &reader);

    void setSubsequenceCfg(uint8_t index, std::unique_ptr<TransformedParameters> cfg);

    Subsequence *getSubsequenceCfg(uint8_t index) const;

    DecoderRegular(const DecoderRegular &c);

    std::unique_ptr<core::parameter::desc_pres::Decoder> clone() const override;

    static std::unique_ptr<core::parameter::desc_pres::DecoderRegular> create(util::BitReader& reader) {
        return util::make_unique<DecoderRegular>(reader);
    }

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