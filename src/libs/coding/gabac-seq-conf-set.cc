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
    for (size_t i = 0; i < MpeggRawAu::getDescriptorProperties().size(); ++i) {
        conf.emplace_back();
        for (size_t j = 0; j < MpeggRawAu::getDescriptorProperties()[i].number_subsequences; ++j) {
            conf[i].emplace_back(DEFAULT_GABAC_CONF_JSON);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const gabac::EncodingConfiguration &GabacSeqConfSet::getConfAsGabac(MpeggRawAu::GenomicDescriptor desc,
                                                                    size_t sub) const {
    return conf[uint8_t(desc)][sub];
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<GabacSeqConfSet::TransformSubseqParameters> GabacSeqConfSet::transParamsFromGabac(
    MpeggRawAu::GenomicDescriptor desc, size_t sub) const {
    using namespace format::mpegg_p2::desc_conf_pres::cabac;

    const auto &gabac_configuration = getConfAsGabac(desc, sub);

    // Build parameters
    auto mpeg_transform_id = TransformSubseqParameters::TransformIdSubseq(gabac_configuration.sequenceTransformationId);
    auto trans_param = gabac_configuration.sequenceTransformationParameter;
    return util::make_unique<TransformSubseqParameters>(mpeg_transform_id, trans_param);
}

// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::TransformIdSubsym GabacSeqConfSet::inferTransform(
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

std::unique_ptr<GabacSeqConfSet::CabacBinarization> GabacSeqConfSet::inferBinarization(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    using namespace format::mpegg_p2::desc_conf_pres::cabac;

    auto bin_ID = CabacBinarizationParameters::BinarizationId(tSeqConf.binarizationId);
    auto bin_params = util::make_unique<CabacBinarizationParameters>(bin_ID, tSeqConf.binarizationParameters[0]);
    auto binarization = util::make_unique<CabacBinarization>(bin_ID, std::move(bin_params));

    // Additional parameters for context adaptive modes
    if (tSeqConf.contextSelectionId != gabac::ContextSelectionId::bypass) {
        auto context_params = util::make_unique<CabacContextParameters>(false, 3, 3, false);
        binarization->setContextParameters(std::move(context_params));  // TODO insert actual values
    }
    return binarization;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::fillSubseqFromGabac(MpeggRawAu::GenomicDescriptor desc, size_t sub,
                                          GabacSeqConfSet::DescriptorSubsequenceCfg *sub_conf) const {
    using namespace format::mpegg_p2::desc_conf_pres::cabac;

    const auto &gabac_configuration = getConfAsGabac(desc, sub);
    size_t trans_seq_id = 0;
    for (const auto &tSeqConf : gabac_configuration.transformedSequenceConfigurations) {
        auto size = gabac::getTransformation(gabac_configuration.sequenceTransformationId).wordsizes[trans_seq_id] * 8;
        if (!size) {
            size = gabac_configuration.wordSize * 8;
        }

        auto transform = inferTransform(tSeqConf);
        auto binarization = inferBinarization(tSeqConf);
        auto supp_vals = util::make_unique<SupportValues>(size, size, 0, transform);
        auto subcfg = util::make_unique<TransformSubseqCfg>(transform, std::move(supp_vals), std::move(binarization));
        sub_conf->setTransformSubseqCfg(trans_seq_id, std::move(subcfg));
        ++trans_seq_id;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::storeParameters(format::mpegg_p2::ParameterSet *parameterSet) const {
    using namespace format::mpegg_p2::desc_conf_pres;

    for (uint8_t descriptor_id = 0; descriptor_id < MpeggRawAu::NUM_DESCRIPTORS; ++descriptor_id) {
        auto mpegg_descriptor_id = MpeggRawAu::GenomicDescriptor(descriptor_id);
        auto decoder_config = util::make_unique<cabac::DecoderConfigurationCabac>(mpegg_descriptor_id);

        const auto MPEGG_NUM_SUBDESCRIPTORS = MpeggRawAu::getDescriptorProperties()[descriptor_id].number_subsequences;
        for (uint8_t sub_desc = 0; sub_desc < MPEGG_NUM_SUBDESCRIPTORS; ++sub_desc) {
            auto transform_params = transParamsFromGabac(mpegg_descriptor_id, sub_desc);
            decoder_config->setSubsequenceCfg(sub_desc, std::move(transform_params));

            // This is where actual translation of one gabac config to MPEGG takes place
            auto subseq_cfg = decoder_config->getSubsequenceCfg(sub_desc);
            fillSubseqFromGabac(mpegg_descriptor_id, sub_desc, subseq_cfg);
        }
        auto descriptor_configuration = util::make_unique<DescriptorConfigurationPresent>();
        descriptor_configuration->set_decoder_configuration(std::move(decoder_config));

        auto descriptor_container = util::make_unique<format::mpegg_p2::DescriptorConfigurationContainer>();
        descriptor_container->setConfig(std::move(descriptor_configuration));

        parameterSet->setDescriptor(descriptor_id, std::move(descriptor_container));
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
