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

void GabacSeqConfSet::setConfAsGabac(core::GenSubIndex sub, DescriptorSubsequenceCfg &subseqCfg) const {
    // FIXME conf[uint8_t(sub.first)][uint8_t(sub.second)].subseq = subseqCfg;
}

#if 0 //RESTRUCT_DISABLE
// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::TransformSubseqParameters GabacSeqConfSet::storeTransParams(
    const gabac::EncodingConfiguration &gabac_configuration) {
    using namespace entropy::paramcabac;

    // Build parameter
    auto mpeg_transform_id = TransformedParameters::TransformIdSubseq(gabac_configuration.sequenceTransformationId);
    auto trans_param = gabac_configuration.sequenceTransformationParameter;
    return TransformedParameters(mpeg_transform_id, trans_param);
}

// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::TransformIdSubsym GabacSeqConfSet::storeTransform(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    using namespace entropy::paramcabac;

    SupportValues::TransformIdSubsym transform = SupportValues::TransformIdSubsym::NO_TRANSFORM;
    if (tSeqConf.lutTransformationEnabled && tSeqConf.diffCodingEnabled) {
        UTILS_THROW_RUNTIME_EXCEPTION("LUT and Diff core at the same time not supported");
    } else if (tSeqConf.lutTransformationEnabled) {
        transform = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
    } else if (tSeqConf.diffCodingEnabled) {
        transform = SupportValues::TransformIdSubsym::DIFF_CODING;
    }
    return transform;
}

// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::CabacBinarization GabacSeqConfSet::storeBinarization(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    using namespace entropy::paramcabac;

    auto bin_ID = BinarizationParameters::BinarizationId(tSeqConf.binarizationId);
    auto bin_params = BinarizationParameters(bin_ID, std::vector<uint8_t>(tSeqConf.binarizationParameters[0]));
    auto binarization = Binarization(bin_ID, std::move(bin_params));

    // Additional parameter for context adaptive modes
    if (tSeqConf.contextSelectionId != gabac::ContextSelectionId::bypass) {
        // TODO insert actual values when adaptive core ready
        auto context_params = Context(false, 3, 3, false);
        binarization.setContextParameters(std::move(context_params));
    }
    return binarization;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::storeSubseq(const gabac::EncodingConfiguration &gabac_configuration,
                                  DescriptorSubsequenceCfg &sub_conf) {
    using namespace entropy::paramcabac;

    size_t trans_seq_id = 0;
    for (const auto &tSeqConf : gabac_configuration.transformedSequenceConfigurations) {
        auto size = gabac::getTransformation(gabac_configuration.sequenceTransformationId).wordsizes[trans_seq_id] * 8;
        size = size ? size : gabac_configuration.wordSize * 8;

        auto transform = storeTransform(tSeqConf);
        auto binarization = storeBinarization(tSeqConf);
        auto supp_vals = SupportValues(size, size, 0, transform);
        auto subcfg = TransformedSeq(transform, std::move(supp_vals), std::move(binarization));
        sub_conf.setTransformSubseqCfg(trans_seq_id, std::move(subcfg));
        ++trans_seq_id;
    }
}

#endif
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

#if 0 //RESTRUCT_DISABLE
// ---------------------------------------------------------------------------------------------------------------------

gabac::TransformedSequenceConfiguration GabacSeqConfSet::loadTransformedSequence(
    const TransformSubseqCfg &transformedDesc) {
    using namespace entropy::paramcabac;
    gabac::TransformedSequenceConfiguration gabacTransCfg;

    const auto DIFF = SupportValues::TransformIdSubsym::DIFF_CODING;
    gabacTransCfg.diffCodingEnabled = transformedDesc.getTransformID() == DIFF;

    const auto LUT = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
    gabacTransCfg.lutTransformationEnabled = transformedDesc.getTransformID() == LUT;

    const auto MAX_BIN = BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB;
    UTILS_DIE_IF(transformedDesc.getBinarization().getBinarizationID() > MAX_BIN, "Binarization unsupported");

    const auto CUR_BIN = transformedDesc.getBinarization().getBinarizationID();
    gabacTransCfg.binarizationId = gabac::BinarizationId(CUR_BIN);

    gabacTransCfg.binarizationParameters.push_back(32);  // TODO Remove hardcoded value
    if (transformedDesc.getBinarization().getBypassFlag()) {
        gabacTransCfg.contextSelectionId = gabac::ContextSelectionId::bypass;
    } else {
        const auto CODING_ORDER = transformedDesc.getSupportValues().getCodingOrder();
        gabacTransCfg.contextSelectionId = gabac::ContextSelectionId(CODING_ORDER + 1);
    }

    gabacTransCfg.lutOrder = 0;  // TODO Remove hardcoded value
    gabacTransCfg.lutBits = 0;   // TODO Remove hardcoded value

    return gabacTransCfg;
}

// ---------------------------------------------------------------------------------------------------------------------
#endif

void GabacSeqConfSet::loadParameters(const core::parameter::ParameterSet &parameterSet) {
    using namespace entropy::paramcabac;

    for (const auto &desc : core::getDescriptors()) {
        auto &descConfig = loadDescriptorDecoderCfg(parameterSet, desc.id);
        for (const auto &subdesc : getDescriptor(desc.id).subseqs) {
            auto sub_desc = descConfig.getSubsequenceCfg(subdesc.id.second);

            //FIXME setConfAsGabac(subdesc.id.second, sub_desc);

        }
    }
}


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
