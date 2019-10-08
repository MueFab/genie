#include <array>
#include "parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h"

#include "clutter.h"
#include "make_unique.h"
#include "parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h"
#include "parameter_set/descriptor_configuration_present/decoder_configuration.h"
#include "parameter_set/descriptor_configuration_present/descriptor_configuration_present.h"
#include "parameter_set/qv_coding_config_1/qv_coding_config_1.h"
#include <climits>
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
            UTILS_THROW_RUNTIME_EXCEPTION("LUT and Diff coding at the same time not supported");
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
            uint8_t size = gabac::getTransformation(conf.sequenceTransformationId).wordsizes[i] * 8;
            if(!size) {
                size = conf.wordSize * 8;
            }

            auto transform = inferTransform(tSeqConf);
            auto binarization = inferBinarization(tSeqConf);
            auto subcfg = make_unique<desc_conf_pres::cabac::TransformSubseqCfg>(
                    transform, make_unique<desc_conf_pres::cabac::SupportValues>(size, size, 0, transform),
                    std::move(binarization));
            sub_conf->setTransformSubseqCfg(i, std::move(subcfg));
            ++i;
        }
    }

    ParameterSet createQuickParameterSet(uint8_t _parameter_set_id, uint8_t _read_length, bool paired_end,
                                         bool qv_values_present, DataUnit::DatasetType dataType,
                                         const std::vector<std::vector<gabac::EncodingConfiguration>> &parameters) {
        ParameterSet ret(_parameter_set_id, _parameter_set_id, dataType,
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

    void write32bit(uint32_t val, std::vector<uint8_t> *ret) {
        for(int i = sizeof(uint32_t) - 1; i >= 0 ; --i) {
            uint8_t writeVal = (val >> i*8) & 0xff;
            ret->push_back(writeVal);
        }
    }


    void insert(gabac::DataBlock *transformed_subsequence, std::vector<uint8_t> *ret) {
        ret->insert(ret->end(), reinterpret_cast<const uint8_t *>(transformed_subsequence->getData()),
                    reinterpret_cast<const uint8_t *>(transformed_subsequence->getData()) +
                    transformed_subsequence->getRawSize());
    }

    void insert(std::vector<gabac::DataBlock> *subsequence, std::vector<uint8_t> *ret) {
        for (size_t i = 0; i < subsequence->size(); ++i) {
            std::vector<uint8_t > tmp;
            insert(&(*subsequence)[i], &tmp);
            if(i != subsequence->size() - 1) {
                write32bit(tmp.size(), ret);
            }

            // TODO: insert number of symbols in subsequence if more than one transformed subsequence
            ret->insert(ret->end(), tmp.begin(), tmp.end());
        }
    }

    void insert(std::vector<std::vector<gabac::DataBlock>> *descriptor, std::vector<uint8_t> *ret) {
        for (size_t i = 0; i < descriptor->size(); ++i) {
            std::vector<uint8_t > tmp;
            insert(&(*descriptor)[i], &tmp);
            if(i != descriptor->size() - 1) {
                write32bit(tmp.size(), ret);
            }
            ret->insert(ret->end(), tmp.begin(), tmp.end());
        }
    }

    std::vector<uint8_t> create_payload(std::vector<std::vector<gabac::DataBlock>> *block) {
        std::vector<uint8_t> ret;

        insert(block, &ret);
        return ret;
    }

    bool descriptorEmpty(const std::vector<std::vector<gabac::DataBlock>>& data) {
        for(auto &s : data) {
            if(!s.empty())
                return false;
        }
        return true;
    }

// -----------------------------------------------------------------------------------------------------------------

    AccessUnit createQuickAccessUnit(uint32_t access_unit_id, uint8_t parameter_set_id, uint32_t reads_count,
                                     DataUnit::AuType autype, DataUnit::DatasetType datatype,
                                     std::vector<std::vector<std::vector<gabac::DataBlock>>> *data) {
        AccessUnit au(access_unit_id, parameter_set_id, autype, reads_count,
                      datatype, 32, 32, 0);
        for (size_t descriptor = 0; descriptor < data->size(); ++descriptor) {
            auto &desc = (*data)[descriptor];
            if(descriptorEmpty(desc)){
                continue;
            }
            auto payload = create_payload(&desc);
            au.addBlock(std::unique_ptr<Block>(new Block(descriptor, &payload)));
        }
        return au;
    }
}  // namespace format
