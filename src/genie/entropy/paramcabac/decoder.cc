/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"
#include <genie/util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------
namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

DecoderRegular::DecoderRegular(core::GenDesc desc)
    : core::parameter::desc_pres::DecoderRegular(MODE_CABAC), descriptor_subsequence_cfgs() {
    for (size_t i = 0; i < core::getDescriptors()[uint8_t(desc)].subseqs.size(); ++i) {
        descriptor_subsequence_cfgs.push_back(
            util::make_unique<Subsequence>(util::make_unique<TransformedParameters>(), i, false));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderRegular::DecoderRegular(util::BitReader &reader) : core::parameter::desc_pres::DecoderRegular(MODE_CABAC) {
    uint8_t num_descriptor_subsequence_cfgs = reader.read(8) + 1;
    for (size_t i = 0; i < num_descriptor_subsequence_cfgs; ++i) {
        descriptor_subsequence_cfgs.emplace_back(util::make_unique<Subsequence>(false, reader));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderRegular::setSubsequenceCfg(uint8_t index, std::unique_ptr<TransformedParameters> cfg) {
    descriptor_subsequence_cfgs[uint8_t(index)] = util::make_unique<Subsequence>(std::move(cfg), uint8_t(index), false);
}

// ---------------------------------------------------------------------------------------------------------------------

Subsequence *DecoderRegular::getSubsequenceCfg(uint8_t index) const {
    return descriptor_subsequence_cfgs[uint8_t(index)].get();
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderRegular::DecoderRegular(const DecoderRegular &c) : core::parameter::desc_pres::DecoderRegular(MODE_CABAC) {
    for (const auto &a : c.descriptor_subsequence_cfgs) {
        descriptor_subsequence_cfgs.emplace_back(a->clone());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::Decoder> DecoderRegular::clone() const {
    auto ret = util::make_unique<DecoderRegular>();
    ret->encoding_mode_ID = encoding_mode_ID;
    for (size_t i = 0; i < descriptor_subsequence_cfgs.size(); ++i) {
        ret->descriptor_subsequence_cfgs[i] = descriptor_subsequence_cfgs[i]->clone();
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderRegular::write(util::BitWriter &writer) const {
    core::parameter::desc_pres::Decoder::write(writer);
    writer.write(descriptor_subsequence_cfgs.size() - 1, 8);
    for (auto &i : descriptor_subsequence_cfgs) {
        i->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderTokenType::DecoderTokenType()
    : core::parameter::desc_pres::DecoderTokentype(MODE_CABAC), rle_guard_tokentype(0), descriptor_subsequence_cfgs() {
    for (size_t i = 0; i < 2; ++i) {
        descriptor_subsequence_cfgs.push_back(
            util::make_unique<Subsequence>(util::make_unique<TransformedParameters>(), i, true));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderTokenType::DecoderTokenType(util::BitReader &reader) : core::parameter::desc_pres::DecoderTokentype(MODE_CABAC) {
    uint8_t num_descriptor_subsequence_cfgs = 2;
    rle_guard_tokentype = reader.read(8);
    for (size_t i = 0; i < num_descriptor_subsequence_cfgs; ++i) {
        descriptor_subsequence_cfgs.emplace_back(util::make_unique<Subsequence>(true, reader));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderTokenType::setSubsequenceCfg(uint8_t index, std::unique_ptr<TransformedParameters> cfg) {
    descriptor_subsequence_cfgs[uint8_t(index)] = util::make_unique<Subsequence>(std::move(cfg), uint8_t(index), true);
}

// ---------------------------------------------------------------------------------------------------------------------

Subsequence *DecoderTokenType::getSubsequenceCfg(uint8_t index) const {
    return descriptor_subsequence_cfgs[uint8_t(index)].get();
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderTokenType::DecoderTokenType(const DecoderTokenType &c)
    : core::parameter::desc_pres::DecoderTokentype(MODE_CABAC), rle_guard_tokentype(c.rle_guard_tokentype) {
    for (const auto &a : c.descriptor_subsequence_cfgs) {
        descriptor_subsequence_cfgs.emplace_back(a->clone());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::Decoder> DecoderTokenType::clone() const {
    auto ret = util::make_unique<DecoderTokenType>();
    ret->encoding_mode_ID = encoding_mode_ID;
    ret->rle_guard_tokentype = rle_guard_tokentype;
    for (size_t i = 0; i < descriptor_subsequence_cfgs.size(); ++i) {
        ret->descriptor_subsequence_cfgs[i] = descriptor_subsequence_cfgs[i]->clone();
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderTokenType::write(util::BitWriter &writer) const {
    core::parameter::desc_pres::Decoder::write(writer);
    writer.write(rle_guard_tokentype, 8);
    for (auto &i : descriptor_subsequence_cfgs) {
        i->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DecoderTokenType::getRleGuardTokentype() const { return rle_guard_tokentype; }

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------