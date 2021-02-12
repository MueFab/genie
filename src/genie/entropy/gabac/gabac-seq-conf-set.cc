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
        const GenomicDescriptorProperties &descProp = getDescriptor(desc.id);
        for (size_t i = 0; i < descProp.subseqs.size(); ++i) {
            conf.back().emplace_back(getEncoderConfigManual(descProp.subseqs[i].id));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const gabac::EncodingConfiguration &GabacSeqConfSet::getConfAsGabac(core::GenSubIndex sub) const {
    return conf[uint8_t(sub.first)][uint8_t(sub.second)];
}

// ---------------------------------------------------------------------------------------------------------------------

gabac::EncodingConfiguration &GabacSeqConfSet::getConfAsGabac(core::GenSubIndex sub) {
    if (getDescriptor(sub.first).tokentype) {
        return conf[uint8_t(sub.first)][0];
    } else {
        return conf[uint8_t(sub.first)][uint8_t(sub.second)];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::setConfAsGabac(core::GenSubIndex sub, DescriptorSubsequenceCfg &&subseqCfg) {
    conf[uint8_t(sub.first)][uint8_t(sub.second)].setSubseqConfig(std::move(subseqCfg));
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

        auto descriptor_container = core::parameter::DescriptorSubseqCfg();
        descriptor_container.set(std::move(descriptor_configuration));

        parameterSet.setDescriptor(desc.id, std::move(descriptor_container));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::storeParameters(core::GenDesc desc, core::parameter::DescriptorSubseqCfg &parameterSet) const {
    auto descriptor_configuration = util::make_unique<core::parameter::desc_pres::DescriptorPresent>();
    using namespace entropy::paramcabac;

    if (desc == core::GenDesc::RNAME || desc == core::GenDesc::MSAR) {
        auto decoder_config = util::make_unique<DecoderTokenType>();
        fillDecoder(core::getDescriptor(desc), *decoder_config);
        descriptor_configuration->setDecoder(std::move(decoder_config));
    } else {
        auto decoder_config = util::make_unique<DecoderRegular>(desc);
        fillDecoder(core::getDescriptor(desc), *decoder_config);
        descriptor_configuration->setDecoder(std::move(decoder_config));
    }

    parameterSet = core::parameter::DescriptorSubseqCfg();
    parameterSet.set(std::move(descriptor_configuration));
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::loadParameters(const core::parameter::ParameterSet &parameterSet) {
    using namespace entropy::paramcabac;

    for (const auto &desc : core::getDescriptors()) {
        if (core::getDescriptor(desc.id).tokentype) {
            auto &descConfig = loadDescriptorDecoderCfg<entropy::paramcabac::DecoderTokenType>(parameterSet, desc.id);
            for (const auto &subdesc : getDescriptor(desc.id).subseqs) {
                auto subseqCfg = descConfig.getSubsequenceCfg((uint8_t)subdesc.id.second);

                setConfAsGabac(subdesc.id, std::move(subseqCfg));
            }
        } else {
            auto &descConfig = loadDescriptorDecoderCfg<entropy::paramcabac::DecoderRegular>(parameterSet, desc.id);
            for (const auto &subdesc : getDescriptor(desc.id).subseqs) {
                auto subseqCfg = descConfig.getSubsequenceCfg((uint8_t)subdesc.id.second);

                setConfAsGabac(subdesc.id, std::move(subseqCfg));
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
