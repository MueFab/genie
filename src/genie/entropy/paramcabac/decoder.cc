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
        descriptor_subsequence_cfgs.push_back(Subsequence(TransformedParameters(),
                                                          i,
                                                          false,
                                                          std::vector<TransformedSubSeq>({TransformedSubSeq()})));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderRegular::DecoderRegular(util::BitReader &reader) : core::parameter::desc_pres::DecoderRegular(MODE_CABAC) {
    uint8_t num_descriptor_subsequence_cfgs = reader.read<uint8_t>() + 1;
    for (size_t i = 0; i < num_descriptor_subsequence_cfgs; ++i) {
        descriptor_subsequence_cfgs.emplace_back(Subsequence(false, reader));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderRegular::setSubsequenceCfg(uint8_t index, Subsequence &&cfg) {
    descriptor_subsequence_cfgs[uint8_t(index)] = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

const Subsequence &DecoderRegular::getSubsequenceCfg(uint8_t index) const {
    return descriptor_subsequence_cfgs[uint8_t(index)];
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::Decoder> DecoderRegular::clone() const {
    return util::make_unique<DecoderRegular>(*this);
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderRegular::write(util::BitWriter &writer) const {
    core::parameter::desc_pres::Decoder::write(writer);
    writer.write(descriptor_subsequence_cfgs.size() - 1, 8);
    for (auto &i : descriptor_subsequence_cfgs) {
        i.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderTokenType::DecoderTokenType()
    : core::parameter::desc_pres::DecoderTokentype(MODE_CABAC), rle_guard_tokentype(0), descriptor_subsequence_cfgs() {
    for (size_t i = 0; i < 2; ++i) {
        descriptor_subsequence_cfgs.emplace_back(TransformedParameters(),
                                                 i,
                                                 true,
                                                 std::vector<TransformedSubSeq>({TransformedSubSeq()}));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderTokenType::DecoderTokenType(util::BitReader &reader) : core::parameter::desc_pres::DecoderTokentype(MODE_CABAC) {
    uint8_t num_descriptor_subsequence_cfgs = 2;
    rle_guard_tokentype = reader.read<uint8_t>();
    for (size_t i = 0; i < num_descriptor_subsequence_cfgs; ++i) {
        descriptor_subsequence_cfgs.emplace_back(Subsequence(true, reader));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderTokenType::setSubsequenceCfg(uint8_t index, Subsequence &&cfg) {
    descriptor_subsequence_cfgs[uint8_t(index)] = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

const Subsequence &DecoderTokenType::getSubsequenceCfg(uint8_t index) const {
    return descriptor_subsequence_cfgs[uint8_t(index)];
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::Decoder> DecoderTokenType::clone() const {
    return util::make_unique<DecoderTokenType>(*this);
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderTokenType::write(util::BitWriter &writer) const {
    core::parameter::desc_pres::Decoder::write(writer);
    writer.write(rle_guard_tokentype, 8);
    for (auto &i : descriptor_subsequence_cfgs) {
        i.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DecoderTokenType::getRleGuardTokentype() const { return rle_guard_tokentype; }

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------