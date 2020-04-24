/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "gabac-seq-conf-set.h"
#include <genie/core/parameter/descriptor_present/decoder.h>
#include <genie/core/parameter/descriptor_present/descriptor_present.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::GabacSeqConfSet() {
    // One configuration per subsequence
    for (const auto &desc : core::getDescriptors()) {
        conf.emplace_back();
        for (size_t i = 0; i < getDescriptor(desc.id).subseqs.size(); ++i) {
            conf.back().emplace_back();
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const gabac::EncodingConfiguration &GabacSeqConfSet::getConfAsGabac(core::GenSubIndex sub) const {
    return conf[uint8_t(sub.first)][uint8_t(sub.second)];
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::setConfAsGabac(core::GenSubIndex sub, DescriptorSubsequenceCfg &subseqCfg) {
    conf[uint8_t(sub.first)][uint8_t(sub.second)].setSubseqConfig(subseqCfg);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::storeParameters(core::parameter::ParameterSet &parameterSet) const {
    using namespace entropy::paramcabac;

    for (const auto &desc : core::getDescriptors()) {
        auto descriptor_configuration = util::make_unique<core::parameter::desc_pres::DescriptorPresent>();

        if (desc.id == core::GenDesc::RNAME || desc.id == core::GenDesc::MSAR) {
            auto decoder_config = util::make_unique<DecoderTokenType>();
            fillDecoder(desc, *decoder_config);
            descriptor_configuration->setDecoder(std::move(decoder_config));
        } else {
            auto decoder_config = util::make_unique<DecoderRegular>(desc.id);
            fillDecoder(desc, *decoder_config);
            descriptor_configuration->setDecoder(std::move(decoder_config));
        }

        auto descriptor_container = core::parameter::DescriptorBox();
        descriptor_container.set(std::move(descriptor_configuration));

        parameterSet.setDescriptor(desc.id, std::move(descriptor_container));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const GabacSeqConfSet::DecoderConfigurationCabac &GabacSeqConfSet::loadDescriptorDecoderCfg(
    const GabacSeqConfSet::ParameterSet &parameterSet, core::GenDesc descriptor_id) {
    using namespace entropy::paramcabac;

    auto &curDesc = parameterSet.getDescriptor(descriptor_id);
    UTILS_DIE_IF(curDesc.isClassSpecific(), "Class specific config not supported");
    auto PRESENT = core::parameter::desc_pres::DescriptorPresent::PRESENT;
    auto &base_conf = curDesc.get();
    UTILS_DIE_IF(base_conf.getPreset() != PRESENT, "Config not present");
    auto &decoder_conf =
        reinterpret_cast<const core::parameter::desc_pres::DescriptorPresent &>(base_conf).getDecoder();
    UTILS_DIE_IF(decoder_conf.getMode() != paramcabac::DecoderRegular::MODE_CABAC, "Config is not paramcabac");

    return reinterpret_cast<const DecoderRegular &>(decoder_conf);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::loadParameters(const core::parameter::ParameterSet &parameterSet) {
    using namespace entropy::paramcabac;

    for (const auto &desc : core::getDescriptors()) {
        auto &descConfig = loadDescriptorDecoderCfg(parameterSet, desc.id);
        for (const auto &subdesc : getDescriptor(desc.id).subseqs) {
            auto subseqCfg = descConfig.getSubsequenceCfg(subdesc.id.second);

            //FIXME setConfAsGabac(subdesc.id.second, subseqCfg);
        }
    }
}


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
