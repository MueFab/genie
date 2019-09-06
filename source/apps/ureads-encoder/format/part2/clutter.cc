#include <array>
#include <ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac_tokentype.h>

#include "clutter.h"
#include "ureads-encoder/format/part2/parameter_set/qv_coding_config_1/qv_coding_config_1.h"
#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h"
#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/decoder_configuration.h"
#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac_regular.h"
#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_present/descriptor_configuration_present.h"
#include "make_unique.h"

namespace format {

    const std::array<GenomicDescriptorProperties, NUM_DESCRIPTORS> &getDescriptorProperties() {
        static std::array<GenomicDescriptorProperties, NUM_DESCRIPTORS> prop =
                {{
                         {"pos", 2},
                         {"rcomp", 1},
                         {"flags", 3},
                         {"mmpos", 2},
                         {"mmtype", 3},
                         {"clips", 4},
                         {"ureads", 1},
                         {"rlen", 1},
                         {"pair", 8},
                         {"mscore", 1},
                         {"mmap", 5},
                         {"msar", 1},
                         {"rtype", 1},
                         {"rgroup", 1},
                         {"qv", 0},
                         {"rname", 2},
                         {"rftp", 1},
                         {"rftt", 1}
                 }};
        return prop;
    }

    std::unique_ptr<desc_conf_pres::cabac::DescriptorSubsequenceCfg>
    subseqFromGabac(const gabac::EncodingConfiguration &conf, uint8_t subsequenceIndex) {
        auto transformParams = make_unique<desc_conf_pres::cabac::TransformSubseqParameters>(
                desc_conf_pres::cabac::TransformSubseqParameters::TransformIdSubseq(conf.sequenceTransformationId),
                conf.sequenceTransformationParameter);
        auto sub_conf = make_unique<desc_conf_pres::cabac::DescriptorSubsequenceCfg>(std::move(transformParams), subsequenceIndex, false);
        for (const auto &i : conf.transformedSequenceConfigurations) {
            desc_conf_pres::cabac::SupportValues::TransformIdSubsym transform = desc_conf_pres::cabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
            if (i.lutTransformationEnabled && i.diffCodingEnabled) {
                GENIE_THROW_RUNTIME_EXCEPTION("LUT and Diff coding at the same time not supported");
            } else if (i.lutTransformationEnabled) {
                transform = desc_conf_pres::cabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
            } else if (i.diffCodingEnabled) {
                transform = desc_conf_pres::cabac::SupportValues::TransformIdSubsym::DIFF_CODING;
            }
            auto binarization = make_unique<desc_conf_pres::cabac::CabacBinarization>(
                    desc_conf_pres::cabac::CabacBinarizationParameters::BinarizationId(i.binarizationId),
                    make_unique<desc_conf_pres::cabac::CabacBinarizationParameters>(
                            desc_conf_pres::cabac::CabacBinarizationParameters::BinarizationId(i.binarizationId), i.binarizationParameters[0]
                    )
            );
            if (i.contextSelectionId != gabac::ContextSelectionId::bypass) {
                binarization->setContextParameters(
                        make_unique<desc_conf_pres::cabac::CabacContextParameters>(false, 8, 8, 0)); //TODO insert actual values
            }
            auto subcfg = make_unique<desc_conf_pres::cabac::TransformSubseqCfg>(transform, make_unique<desc_conf_pres::cabac::SupportValues>(8, 8, 0, transform),
                                                          std::move(binarization)); // TODO insert actual values
            sub_conf->addTransformSubseqCfg(std::move(subcfg));
        }
        return sub_conf;
    }

    ParameterSet
    createQuickParameterSet(uint8_t _parameter_set_id, uint8_t _read_length, bool paired_end, bool qv_values_present,
                            const std::vector<std::vector<gabac::EncodingConfiguration>> &parameters) {
        ParameterSet ret(_parameter_set_id,
                         _parameter_set_id,
                         DataUnit::DatasetType::NON_ALIGNED,
                         ParameterSet::AlphabetID::ACGTN,
                         _read_length,
                         paired_end,
                         false,
                         qv_values_present,
                         0,
                         false,
                         false
        );
        ret.addClass(DataUnit::AuType::U_TYPE_AU,
                     make_unique<qv_coding1::QvCodingConfig1>(qv_coding1::QvCodingConfig1::QvpsPresetId::ASCII, false));
        for (int desc = 0; desc < 18; ++desc) {
            std::unique_ptr<desc_conf_pres::cabac::DecoderConfigurationCabac> dcg;
            if (desc != 11 && desc != 15) {
                dcg = make_unique<desc_conf_pres::cabac::DecoderConfigurationCabacRegular>();
            } else {
                dcg = make_unique<desc_conf_pres::cabac::DecoderConfigurationCabacTokentype>();
            }
            for (size_t subseq = 0; subseq < parameters[desc].size(); ++subseq) {
                dcg->addSubsequenceCfg(subseqFromGabac(parameters[desc][subseq], subseq));
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
                      DataUnit::DatasetType::NON_ALIGNED);
        for (size_t i = 0; i < data->size(); ++i) {
            if (i == 11 || i == 15 || data->at(i).empty()) {
                continue; // TODO: Token types
            }
            auto &desc = (*data)[i];
            auto payload = create_payload(desc);
            au.addBlock(std::unique_ptr<Block>(new Block(i, &payload)));
        }
        return au;
    }
}