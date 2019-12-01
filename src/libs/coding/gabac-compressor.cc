#include "gabac-compressor.h"
#include <util/make_unique.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/descriptor_configuration_present.h>


/**
 * Extract transformation information from gabac config
 * @param conf
 * @return
 */
std::unique_ptr<format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqParameters> transParamsFromGabac(
        const gabac::EncodingConfiguration &conf) {
    return util::make_unique<format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqParameters>(
            format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqParameters::TransformIdSubseq(conf.sequenceTransformationId),
            conf.sequenceTransformationParameter);
}

format::mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym inferTransform(
        const gabac::TransformedSequenceConfiguration &tSeqConf) {
    format::mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym transform =
            format::mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
    if (tSeqConf.lutTransformationEnabled && tSeqConf.diffCodingEnabled) {
        UTILS_THROW_RUNTIME_EXCEPTION("LUT and Diff coding at the same time not supported");
    } else if (tSeqConf.lutTransformationEnabled) {
        transform = format::mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
    } else if (tSeqConf.diffCodingEnabled) {
        transform = format::mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym::DIFF_CODING;
    }
    return transform;
}

std::unique_ptr<format::mpegg_p2::desc_conf_pres::cabac::CabacBinarization> inferBinarization(
        const gabac::TransformedSequenceConfiguration &tSeqConf) {
    auto ret = util::make_unique<format::mpegg_p2::desc_conf_pres::cabac::CabacBinarization>(
            format::mpegg_p2::desc_conf_pres::cabac::CabacBinarizationParameters::BinarizationId(tSeqConf.binarizationId),
            util::make_unique<format::mpegg_p2::desc_conf_pres::cabac::CabacBinarizationParameters>(
                    format::mpegg_p2::desc_conf_pres::cabac::CabacBinarizationParameters::BinarizationId(tSeqConf.binarizationId),
                    tSeqConf.binarizationParameters[0]));
    if (tSeqConf.contextSelectionId != gabac::ContextSelectionId::bypass) {
        ret->setContextParameters(
                util::make_unique<format::mpegg_p2::desc_conf_pres::cabac::CabacContextParameters>(false, 3, 3,
                                                                                 false));  // TODO insert actual values
    }
    return ret;
}

/**
 * Fill subsequence information from gabac config
 */
void fillSubseqFromGabac(const gabac::EncodingConfiguration &conf,
                         format::mpegg_p2::desc_conf_pres::cabac::DescriptorSubsequenceCfg *sub_conf) {
    size_t i = 0;
    for (const auto &tSeqConf : conf.transformedSequenceConfigurations) {
        uint8_t size = gabac::getTransformation(conf.sequenceTransformationId).wordsizes[i] * 8;
        if (!size) {
            size = conf.wordSize * 8;
        }

        auto transform = inferTransform(tSeqConf);
        auto binarization = inferBinarization(tSeqConf);
        auto subcfg = util::make_unique<format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqCfg>(
                transform, util::make_unique<format::mpegg_p2::desc_conf_pres::cabac::SupportValues>(size, size, 0, transform),
                std::move(binarization));
        sub_conf->setTransformSubseqCfg(i, std::move(subcfg));
        ++i;
    }
}


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
    for (size_t i = 0; i < MpeggRawAu::getDescriptorProperties().size(); ++i) {
        conf.emplace_back();
        for (size_t j = 0; j < MpeggRawAu::getDescriptorProperties()[i].number_subsequences; ++j) {
            conf[i].emplace_back(DEFAULT_GABAC_CONF_JSON);
        }
    }
}

const gabac::EncodingConfiguration *GabacSeqConfSet::getConf(MpeggRawAu::GenomicDescriptor desc, size_t sub) {
    return &(conf[uint8_t(desc)][sub]);
}


void GabacCompressor::compress(const gabac::EncodingConfiguration &conf, gabac::DataBlock *in,
                               std::vector<gabac::DataBlock> *out) {
    // Interface to GABAC library
    gabac::IBufferStream bufferInputStream(in);
    lae::GenieGabacOutputStream bufferOutputStream;

    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream *const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream, &bufferOutputStream, GABAC_BLOCK_SIZE,
                                                   GABC_LOG_OUTPUT_STREAM,
                                                   gabac::IOConfiguration::LogLevel::TRACE};

    const bool GABAC_DECODING_MODE = false;
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);
    bufferOutputStream.flush_blocks(out);
}

void GabacCompressor::flowIn(std::unique_ptr<MpeggRawAu> t, size_t id) {
    auto payload = util::make_unique<BlockPayloadSet>(t->moveParameters());

    for (size_t desc = 0; desc < MpeggRawAu::NUM_DESCRIPTORS; ++desc) {
        auto descriptor_pay = util::make_unique<DescriptorPayload>();

        std::unique_ptr<format::mpegg_p2::desc_conf_pres::cabac::DecoderConfigurationCabac> dcg =
                util::make_unique<format::mpegg_p2::desc_conf_pres::cabac::DecoderConfigurationCabac>(MpeggRawAu::GenomicDescriptor(desc));

        for (size_t sub_desc = 0;
             sub_desc < MpeggRawAu::getDescriptorProperties()[desc].number_subsequences; ++sub_desc) {

            // Compression

            auto sub_descriptor_pay = util::make_unique<SubDescriptorPayload>();
            auto *inseq = &t->get(MpeggRawAu::GenomicDescriptor(desc), sub_desc);
            gabac::DataBlock in((const uint8_t *) inseq->getData(), inseq->getWordSize(), inseq->rawSize());
            std::vector<gabac::DataBlock> out;
            compress(*configSet.getConf(MpeggRawAu::GenomicDescriptor(desc), sub_desc), &in, &out);

            for (auto &o : out) {
                sub_descriptor_pay->add(util::make_unique<TransformedPayload>(&o));
            }
            descriptor_pay->add(std::move(sub_descriptor_pay));

            // Parameters

            dcg->setSubsequenceCfg(sub_desc, transParamsFromGabac(*configSet.getConf(MpeggRawAu::GenomicDescriptor(desc), sub_desc)));
            fillSubseqFromGabac(*configSet.getConf(MpeggRawAu::GenomicDescriptor(desc), sub_desc), dcg->getSubsequenceCfg(sub_desc));
        }
        if (!descriptor_pay->isEmpty()) {
            payload->setPayload(desc, std::move(descriptor_pay));
        }

        // Parameters

        auto dc = util::make_unique<format::mpegg_p2::desc_conf_pres::DescriptorConfigurationPresent>();
        dc->set_decoder_configuration(std::move(dcg));
        auto d = util::make_unique<format::mpegg_p2::DescriptorConfigurationContainer>();
        d->setConfig(std::move(dc));

        payload->getParameters()->setDescriptor(desc, std::move(d));
    }

    flowOut(std::move(payload), id);
}

void GabacCompressor::dryIn() {
    dryOut();
}



