#include <array>
#include "parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h"

#include "clutter.h"
#include "make_unique.h"
#include "parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h"
#include "parameter_set/descriptor_configuration_present/decoder_configuration.h"
#include "parameter_set/descriptor_configuration_present/descriptor_configuration_present.h"
#include "parameter_set/qv_coding_config_1/qv_coding_config_1.h"

namespace format {

const std::vector<GenomicDescriptorProperties> &getDescriptorProperties() {
    // static std::array<GenomicDescriptorProperties, NUM_DESCRIPTORS> prop =
    //         {{
    //                  {"pos", 2},
    //                  {"rcomp", 1},
    //                  {"flags", 3},
    //                  {"mmpos", 2},
    //                  {"mmtype", 3},
    //                  {"clips", 4},
    //                  {"ureads", 1},
    //                  {"rlen", 1},
    //                  {"pair", 8},
    //                  {"mscore", 1},
    //                  {"mmap", 5},
    //                  {"msar", 2},
    //                  {"rtype", 1},
    //                  {"rgroup", 1},
    //                  {"qv", 1},
    //                  {"rname", 2},
    //                  {"rftp", 1},
    //                  {"rftt", 1}
    //          }};
    static const auto prop = []() -> std::vector<GenomicDescriptorProperties> {
        std::vector<GenomicDescriptorProperties> loc;
        loc.push_back({"pos", 2});
        loc.push_back({"rcomp", 1});
        loc.push_back({"flags", 3});
        loc.push_back({"mmpos", 2});
        loc.push_back({"mmtype", 3});
        loc.push_back({"clips", 4});
        loc.push_back({"ureads", 1});
        loc.push_back({"rlen", 1});
        loc.push_back({"pair", 8});
        loc.push_back({"mscore", 1});
        loc.push_back({"mmap", 5});
        loc.push_back({"msar", 2});
        loc.push_back({"rtype", 1});
        loc.push_back({"rgroup", 1});
        loc.push_back({"qv", 1});
        loc.push_back({"rname", 2});
        loc.push_back({"rftp", 1});
        loc.push_back({"rftt", 1});
        return loc;
    }();
    return prop;
}

/**
 * Extract transformation information from gabac config
 * @param conf
 * @return
 */
std::unique_ptr<desc_conf_pres::cabac::TransformSubseqParameters> transParamsFromGabac(
    const gabac::EncodingConfiguration &conf) {
    return make_unique<desc_conf_pres::cabac::TransformSubseqParameters>(
        desc_conf_pres::cabac::TransformSubseqParameters::TransformIdSubseq(conf.sequenceTransformationId),
        conf.sequenceTransformationParameter);
}

desc_conf_pres::cabac::SupportValues::TransformIdSubsym inferTransform(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    desc_conf_pres::cabac::SupportValues::TransformIdSubsym transform =
        desc_conf_pres::cabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
    if (tSeqConf.lutTransformationEnabled && tSeqConf.diffCodingEnabled) {
        GENIE_THROW_RUNTIME_EXCEPTION("LUT and Diff coding at the same time not supported");
    } else if (tSeqConf.lutTransformationEnabled) {
        transform = desc_conf_pres::cabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
    } else if (tSeqConf.diffCodingEnabled) {
        transform = desc_conf_pres::cabac::SupportValues::TransformIdSubsym::DIFF_CODING;
    }
    return transform;
}

std::unique_ptr<desc_conf_pres::cabac::CabacBinarization> inferBinarization(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    auto ret = make_unique<desc_conf_pres::cabac::CabacBinarization>(
        desc_conf_pres::cabac::CabacBinarizationParameters::BinarizationId(tSeqConf.binarizationId),
        make_unique<desc_conf_pres::cabac::CabacBinarizationParameters>(
            desc_conf_pres::cabac::CabacBinarizationParameters::BinarizationId(tSeqConf.binarizationId),
            tSeqConf.binarizationParameters[0]));
    if (tSeqConf.contextSelectionId != gabac::ContextSelectionId::bypass) {
        ret->setContextParameters(
            make_unique<desc_conf_pres::cabac::CabacContextParameters>(false, 3, 3,
                                                                       false));  // TODO insert actual values
    }
    return ret;
}

/**
 * Fill subsequence information from gabac config
 */
void fillSubseqFromGabac(const gabac::EncodingConfiguration &conf,
                         desc_conf_pres::cabac::DescriptorSubsequenceCfg *sub_conf) {
    size_t i = 0;
    for (const auto &tSeqConf : conf.transformedSequenceConfigurations) {
        auto transform = inferTransform(tSeqConf);
        auto binarization = inferBinarization(tSeqConf);
        auto subcfg = make_unique<desc_conf_pres::cabac::TransformSubseqCfg>(
            transform, make_unique<desc_conf_pres::cabac::SupportValues>(3, 3, 0, transform),
            std::move(binarization));  // TODO insert actual values
        sub_conf->setTransformSubseqCfg(i, std::move(subcfg));
        ++i;
    }
}

ParameterSet createQuickParameterSet(uint8_t _parameter_set_id, uint8_t _read_length, bool paired_end,
                                     bool qv_values_present,
                                     const std::vector<std::vector<gabac::EncodingConfiguration>> &parameters) {
    ParameterSet ret(_parameter_set_id, _parameter_set_id, DataUnit::DatasetType::NON_ALIGNED,
                     ParameterSet::AlphabetID::ACGTN, _read_length, paired_end, false, qv_values_present, 0, false,
                     false);
    ret.addClass(DataUnit::AuType::U_TYPE_AU,
                 make_unique<qv_coding1::QvCodingConfig1>(qv_coding1::QvCodingConfig1::QvpsPresetId::ASCII, false));
    for (size_t desc = 0; desc < NUM_DESCRIPTORS; ++desc) {
        std::unique_ptr<desc_conf_pres::cabac::DecoderConfigurationCabac> dcg =
            make_unique<desc_conf_pres::cabac::DecoderConfigurationCabac>(GenomicDescriptor(desc));

        for (size_t subseq = 0; subseq < getDescriptorProperties()[uint8_t(desc)].number_subsequences; ++subseq) {
            dcg->setSubsequenceCfg(subseq, transParamsFromGabac(parameters[desc][subseq]));
            fillSubseqFromGabac(parameters[desc][subseq], dcg->getSubsequenceCfg(subseq));
        }

        auto dc = make_unique<desc_conf_pres::DescriptorConfigurationPresent>();
        dc->set_decoder_configuration(std::move(dcg));
        auto d = make_unique<DescriptorConfigurationContainer>();
        d->setConfig(std::move(dc));

        ret.setDescriptor(desc, std::move(d));
    }
    return ret;
}

std::vector<uint8_t> create_payload(const std::vector<gabac::DataBlock> &block) {
    std::vector<uint8_t> ret;
    uint64_t totalSize = 0;
    for (auto &load : block) {
        totalSize += load.getRawSize();
        totalSize += sizeof(uint32_t);
    }
    if (block.size() > 0) {
        totalSize -= sizeof(uint32_t);
    }
    ret.reserve(totalSize);

    for (size_t i = 0; i < block.size(); ++i) {
        auto &load = block[i];
        if (i != block.size() - 1) {
            uint32_t size = load.getRawSize();
            ret.insert(ret.end(), reinterpret_cast<uint8_t *>(&size),
                       reinterpret_cast<uint8_t *>(&size) + sizeof(uint32_t));
        }
        if (load.getRawSize()) {
            ret.insert(ret.end(), reinterpret_cast<const uint8_t *>(load.getData()),
                       reinterpret_cast<const uint8_t *>(load.getData()) + load.getRawSize());
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------------------------------------------

AccessUnit createQuickAccessUnit(uint32_t access_unit_id, uint8_t parameter_set_id, uint32_t reads_count,
                                 std::vector<std::vector<gabac::DataBlock>> *data) {
    AccessUnit au(access_unit_id, parameter_set_id, DataUnit::AuType::U_TYPE_AU, reads_count,
                  DataUnit::DatasetType::NON_ALIGNED, 32, 32, 0);
    for (size_t i = 0; i < data->size(); ++i) {
        if (i != 6) continue;
        auto &desc = (*data)[i];
        auto payload = create_payload(desc);
        au.addBlock(std::unique_ptr<Block>(new Block(i, &payload)));
    }
    return au;
}
}  // namespace format
