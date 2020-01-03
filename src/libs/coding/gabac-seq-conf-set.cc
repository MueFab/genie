/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "gabac-seq-conf-set.h"
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/support_values.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/descriptor_configuration_present.h>
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::GabacSeqConfSet() {
    const std::string DEFAULT_GABAC_CONF_JSON =
        "{"
        "\"word_size\": 4,"
        "\"sequence_transformation_id\": 0,"
        "\"sequence_transformation_parameter\": 0,"
        "\"transformed_sequences\":"
        "[{"
        "\"lut_transformation_enabled\": false,"
        "\"diff_coding_enabled\": false,"
        "\"binarization_id\": 0,"
        "\"binarization_parameters\":[32],"
        "\"context_selection_id\": 0"
        "}]"
        "}";

    // One configuration per subsequence
    for (const auto &desc : getDescriptors()) {
        conf.emplace_back();
        for (size_t i = 0; i < getDescriptor(desc.id).subseqs.size(); ++i) {
            conf.back().emplace_back(DEFAULT_GABAC_CONF_JSON);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const gabac::EncodingConfiguration &GabacSeqConfSet::getConfAsGabac(GenSubIndex sub) const {
    return conf[uint8_t(sub.first)][uint8_t(sub.second)];
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<GabacSeqConfSet::TransformSubseqParameters> GabacSeqConfSet::storeTransParams(
    const gabac::EncodingConfiguration &gabac_configuration) {
    using namespace format::mpegg_p2::desc_conf_pres::cabac;

    // Build parameters
    auto mpeg_transform_id = TransformSubseqParameters::TransformIdSubseq(gabac_configuration.sequenceTransformationId);
    auto trans_param = gabac_configuration.sequenceTransformationParameter;
    return util::make_unique<TransformSubseqParameters>(mpeg_transform_id, trans_param);
}

// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::TransformIdSubsym GabacSeqConfSet::storeTransform(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    using namespace format::mpegg_p2::desc_conf_pres::cabac;

    SupportValues::TransformIdSubsym transform = SupportValues::TransformIdSubsym::NO_TRANSFORM;
    if (tSeqConf.lutTransformationEnabled && tSeqConf.diffCodingEnabled) {
        UTILS_THROW_RUNTIME_EXCEPTION("LUT and Diff coding at the same time not supported");
    } else if (tSeqConf.lutTransformationEnabled) {
        transform = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
    } else if (tSeqConf.diffCodingEnabled) {
        transform = SupportValues::TransformIdSubsym::DIFF_CODING;
    }
    return transform;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<GabacSeqConfSet::CabacBinarization> GabacSeqConfSet::storeBinarization(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    using namespace format::mpegg_p2::desc_conf_pres::cabac;

    auto bin_ID = CabacBinarizationParameters::BinarizationId(tSeqConf.binarizationId);
    auto bin_params = util::make_unique<CabacBinarizationParameters>(bin_ID, tSeqConf.binarizationParameters[0]);
    auto binarization = util::make_unique<CabacBinarization>(bin_ID, std::move(bin_params));

    // Additional parameters for context adaptive modes
    if (tSeqConf.contextSelectionId != gabac::ContextSelectionId::bypass) {
        // TODO insert actual values when adaptive coding ready
        auto context_params = util::make_unique<CabacContextParameters>(false, 3, 3, false);
        binarization->setContextParameters(std::move(context_params));
    }
    return binarization;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::storeSubseq(const gabac::EncodingConfiguration &gabac_configuration,
                                  DescriptorSubsequenceCfg &sub_conf) {
    using namespace format::mpegg_p2::desc_conf_pres::cabac;

    size_t trans_seq_id = 0;
    for (const auto &tSeqConf : gabac_configuration.transformedSequenceConfigurations) {
        auto size = gabac::getTransformation(gabac_configuration.sequenceTransformationId).wordsizes[trans_seq_id] * 8;
        size = size ? size : gabac_configuration.wordSize * 8;

        auto transform = storeTransform(tSeqConf);
        auto binarization = storeBinarization(tSeqConf);
        auto supp_vals = util::make_unique<SupportValues>(size, size, 0, transform);
        auto subcfg = util::make_unique<TransformSubseqCfg>(transform, std::move(supp_vals), std::move(binarization));
        sub_conf.setTransformSubseqCfg(trans_seq_id, std::move(subcfg));
        ++trans_seq_id;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::storeParameters(format::mpegg_p2::ParameterSet &parameterSet) const {
    using namespace format::mpegg_p2::desc_conf_pres;
    using namespace format::mpegg_p2;

    for (const auto &desc : getDescriptors()) {
        auto decoder_config = util::make_unique<cabac::DecoderConfigurationCabac>(desc.id);

        for (const auto &subdesc : desc.subseqs) {
            auto transform_params = storeTransParams(getConfAsGabac(subdesc.id));
            decoder_config->setSubsequenceCfg(subdesc.id.second, std::move(transform_params));

            // This is where actual translation of one gabac config to MPEGG takes place
            auto subseq_cfg = decoder_config->getSubsequenceCfg(subdesc.id.second);
            storeSubseq(getConfAsGabac(subdesc.id), *subseq_cfg);
        }
        auto descriptor_configuration = util::make_unique<DescriptorConfigurationPresent>();
        descriptor_configuration->set_decoder_configuration(std::move(decoder_config));

        auto descriptor_container = util::make_unique<DescriptorConfigurationContainer>();
        descriptor_container->setConfig(std::move(descriptor_configuration));

        parameterSet.setDescriptor(desc.id, std::move(descriptor_container));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const GabacSeqConfSet::DecoderConfigurationCabac &GabacSeqConfSet::loadDescriptorDecoderCfg(
    const GabacSeqConfSet::ParameterSet &parameterSet, GenDesc descriptor_id) {
    using namespace format::mpegg_p2::desc_conf_pres;
    using namespace format::mpegg_p2;

    auto curDesc = parameterSet.getDescriptor(descriptor_id);
    if (curDesc->isClassSpecific()) {
        UTILS_DIE("Class specific config not supported");
    }
    auto PRESENT = DescriptorConfiguration::DecCfgPreset::PRESENT;
    auto base_conf = curDesc->getConfig();
    if (base_conf->getType() != PRESENT) {
        UTILS_DIE("Config not present");
    }
    auto decoder_conf = reinterpret_cast<const DescriptorConfigurationPresent &>(*base_conf).getDecoderConfiguration();
    if (decoder_conf->getEncodingMode() != DecoderConfiguration::EncodingModeId::CABAC) {
        UTILS_DIE("Config is not cabac");
    }

    return reinterpret_cast<const cabac::DecoderConfigurationCabac &>(*decoder_conf);
}

// ---------------------------------------------------------------------------------------------------------------------

gabac::TransformedSequenceConfiguration GabacSeqConfSet::loadTransformedSequence(const TransformSubseqCfg& transformedDesc) {
    using namespace format::mpegg_p2::desc_conf_pres;
    using namespace format::mpegg_p2::desc_conf_pres::cabac;
    gabac::TransformedSequenceConfiguration gabacTransCfg;

    const auto DIFF = SupportValues::TransformIdSubsym::DIFF_CODING;
    gabacTransCfg.diffCodingEnabled = transformedDesc.getTransformID() == DIFF;

    const auto LUT = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
    gabacTransCfg.lutTransformationEnabled = transformedDesc.getTransformID() == LUT;

    const auto MAX_BIN = CabacBinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB;
    if (transformedDesc.getBinarization()->getBinarizationID() > MAX_BIN) {
        UTILS_DIE("Binarization unsupported");
    }

    const auto CUR_BIN = transformedDesc.getBinarization()->getBinarizationID();
    gabacTransCfg.binarizationId = gabac::BinarizationId(CUR_BIN);

    gabacTransCfg.binarizationParameters.push_back(32);  // TODO Remove hardcoded value
    if (transformedDesc.getBinarization()->getBypassFlag()) {
        gabacTransCfg.contextSelectionId = gabac::ContextSelectionId::bypass;
    } else {
        const auto CODING_ORDER = transformedDesc.getSupportValues()->getCodingOrder();
        gabacTransCfg.contextSelectionId = gabac::ContextSelectionId(CODING_ORDER + 1);
    }

    gabacTransCfg.lutOrder = 0;  // TODO Remove hardcoded value
    gabacTransCfg.lutBits = 0;   // TODO Remove hardcoded value

    return gabacTransCfg;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::loadParameters(const format::mpegg_p2::ParameterSet &parameterSet) {
    using namespace format::mpegg_p2::desc_conf_pres;
    using namespace format::mpegg_p2::desc_conf_pres::cabac;

    for (const auto &desc : getDescriptors()) {
        auto& descConfig = loadDescriptorDecoderCfg(parameterSet, desc.id);
        for (const auto &subdesc : getDescriptor(desc.id).subseqs) {
            auto sub_desc = descConfig.getSubsequenceCfg(subdesc.id.second);
            auto sub_desc_id = desc.tokentype ? 0 : sub_desc->getDescriptorSubsequenceID();

            auto &gabac_conf = conf[uint8_t(desc.id)][sub_desc_id];

            gabac_conf.wordSize = 4;  // TODO Remove hardcoded value

            const auto TRANS_ID = sub_desc->getTransformParameters()->getTransformIdSubseq();
            gabac_conf.sequenceTransformationId = gabac::SequenceTransformationId(TRANS_ID);

            for (const auto &transformedDesc : sub_desc->getTransformSubseqCfgs()) {
                gabac_conf.transformedSequenceConfigurations.emplace_back();
                auto &gabacTransCfg = gabac_conf.transformedSequenceConfigurations.back();

                gabacTransCfg = loadTransformedSequence(*transformedDesc);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
