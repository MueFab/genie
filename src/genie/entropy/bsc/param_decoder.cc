/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/bsc/param_decoder.h"
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

// ---------------------------------------------------------------------------------------------------------------------

DecoderRegular::DecoderRegular() : core::parameter::desc_pres::DecoderRegular(MODE_BSC) {}

// ---------------------------------------------------------------------------------------------------------------------

DecoderRegular::DecoderRegular(core::GenDesc desc) : core::parameter::desc_pres::DecoderRegular(MODE_BSC) {
    for (size_t i = 0; i < core::getDescriptors()[static_cast<uint8_t>(desc)].subseqs.size(); ++i) {
        auto bits_p2 = core::range2bytes(getDescriptor(desc).subseqs[i].range);
        descriptor_subsequence_cfgs.emplace_back(bits_p2);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DecoderRegular::DecoderRegular(core::GenDesc, util::BitReader &reader)
    : core::parameter::desc_pres::DecoderRegular(MODE_BSC) {
    const uint8_t num_descriptor_subsequence_cfgs = reader.read<uint8_t>() + 1;
    for (size_t i = 0; i < num_descriptor_subsequence_cfgs; ++i) {
        descriptor_subsequence_cfgs.emplace_back(reader.read<uint8_t>(6));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderRegular::setSubsequenceCfg(const uint8_t index, Subsequence &&cfg) {
    descriptor_subsequence_cfgs[index] = cfg;
}

// ---------------------------------------------------------------------------------------------------------------------

const Subsequence &DecoderRegular::getSubsequenceCfg(const uint8_t index) const {
    return descriptor_subsequence_cfgs[index];
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::Decoder> DecoderRegular::clone() const {
    return std::make_unique<DecoderRegular>(*this);
}

// ---------------------------------------------------------------------------------------------------------------------

Subsequence &DecoderRegular::getSubsequenceCfg(const uint8_t index) { return descriptor_subsequence_cfgs[index]; }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::parameter::desc_pres::DecoderRegular> DecoderRegular::create(core::GenDesc desc,
                                                                                   util::BitReader &reader) {
    return std::make_unique<DecoderRegular>(desc, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

void DecoderRegular::write(util::BitWriter &writer) const {
    Decoder::write(writer);
    writer.writeBits(descriptor_subsequence_cfgs.size() - 1, 8);
    for (auto &i : descriptor_subsequence_cfgs) {
        i.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool DecoderRegular::equals(const Decoder *dec) const {
    return Decoder::equals(dec) &&
           dynamic_cast<const DecoderRegular *>(dec)->descriptor_subsequence_cfgs == descriptor_subsequence_cfgs;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
