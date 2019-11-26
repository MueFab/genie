#include <array>
#include "parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h"

#include <algorithm>
#include <climits>
#include "clutter.h"
#include "make_unique.h"
#include "parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h"
#include "parameter_set/descriptor_configuration_present/decoder_configuration.h"
#include "parameter_set/descriptor_configuration_present/descriptor_configuration_present.h"
#include "parameter_set/qv_coding_config_1/qv_coding_config_1.h"
#include <climits>

#include <algorithm>
#include <iterator>     // std::advance

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
            loc.push_back({"qv", 3});
            loc.push_back({"rname", 2});
            loc.push_back({"rftp", 1});
            loc.push_back({"rftt", 1});
            return loc;
        }();
        return prop;
    }

    const Alphabet &getAlphabetProperties(ParameterSet::AlphabetID id) {
        static const auto prop = []() -> std::vector<Alphabet> {
            std::vector<Alphabet> loc;
            loc.emplace_back();
            loc.back().lut = {'A', 'C', 'G', 'T', 'N'};
            loc.emplace_back();
            loc.back().lut = {'A', 'C', 'G', 'T', 'R', 'Y', 'S', 'W', 'K', 'M', 'B', 'D', 'H', 'V', 'N', '-'};
            for (auto &l : loc) {
                l.inverseLut = std::vector<char>(*std::max_element(l.lut.begin(), l.lut.end()) + 1, 0);
                for (size_t i = 0; i < l.lut.size(); ++i) {
                    l.inverseLut[l.lut[i]] = i;
                }
            }
            return loc;
        }();
    return prop[uint8_t(id)];
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
            if (!size) {
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
                                         bool qv_values_present, DataUnit::AuType type,
                                         const std::vector<std::vector<gabac::EncodingConfiguration>> &parameters,
                                         const bool reverse_flag) {
        DataUnit::DatasetType dataType = (type == DataUnit::AuType::U_TYPE_AU) ? DataUnit::DatasetType::NON_ALIGNED
                                                                               : DataUnit::DatasetType::ALIGNED;
        ParameterSet ret(_parameter_set_id, _parameter_set_id, dataType,
                         ParameterSet::AlphabetID::ACGTN, _read_length, paired_end, false, qv_values_present, 0, false,
                         false);
        ret.addClass(type,
                     make_unique<qv_coding1::QvCodingConfig1>(qv_coding1::QvCodingConfig1::QvpsPresetId::ASCII,
                                                              reverse_flag));
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

    void write32bit(uint32_t
    val,
    std::vector<uint8_t> *ret
    ) {
    for(
    int i = sizeof(uint32_t) - 1;
    i >= 0; --i) {
    uint8_t writeVal = (val >> i * 8) & 0xff;
    ret->
    push_back(writeVal);
}
}

void write16bit(uint16_t val, std::vector<uint8_t> *ret) {
    for (int i = sizeof(uint16_t) - 1; i >= 0; --i) {
        uint8_t writeVal = (val >> i * 8) & 0xff;
        ret->push_back(writeVal);
    }
}

void write8bit(uint8_t val, std::vector<uint8_t> *ret) {
    ret->push_back(val);
}

void writeu7v(uint32_t val, std::vector<uint8_t> *ret) {
    std::vector<uint8_t> tmp;
    do {
        tmp.push_back(val & 0x7f);
        val >>= 7;
    } while (val != 0);
    for (int i = tmp.size() - 1; i > 0; i--)
        ret->push_back(tmp[i] | 0x80);
    ret->push_back(tmp[0]);
}

void insert(gabac::DataBlock *transformed_subsequence, std::vector<uint8_t> *ret) {
    ret->insert(ret->end(), reinterpret_cast<const uint8_t *>(transformed_subsequence->getData()),
                reinterpret_cast<const uint8_t *>(transformed_subsequence->getData()) +
                transformed_subsequence->getRawSize());
}

void insert(std::vector<gabac::DataBlock> *subsequence, std::vector<uint8_t> *ret) {
    for (size_t i = 0; i < subsequence->size(); ++i) {
        std::vector<uint8_t> tmp;
        insert(&(*subsequence)[i], &tmp);
        if (i != subsequence->size() - 1) {
            write32bit(tmp.size(), ret);
        }

        // TODO: insert number of symbols in subsequence if more than one transformed subsequence
        ret->insert(ret->end(), tmp.begin(), tmp.end());
    }
}

void insert(std::vector<std::vector<gabac::DataBlock>> *descriptor, std::vector<uint8_t> *ret) {
    for (size_t i = 0; i < descriptor->size(); ++i) {
        std::vector<uint8_t> tmp;
        insert(&(*descriptor)[i], &tmp);
        if (i != descriptor->size() - 1) {
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

std::vector<uint8_t>
create_payload_tokentype(std::vector<std::vector<gabac::DataBlock>> *block, uint32_t records_count) {
    std::vector<uint8_t> ret;
    if (block->size() != 128 * 6)
        throw std::runtime_error("tokentype: number of tokens not 128*6 as expected.");
    write32bit(records_count, &ret);
    uint16_t num_tokentype_sequences = 0;
    // assumes standard id_tokenization used
    for (uint16_t i = 0; i < 128 * 6; i++) {
        if ((*block)[i].size() != 0)
            num_tokentype_sequences++;
    }
    write16bit(num_tokentype_sequences, &ret);
    for (uint16_t i = 0; i < 128 * 6; i++) {
        if ((*block)[i].size() == 0)
            continue;
        uint8_t type_id = i % 6;
        uint8_t method_id = 3; // CABAC_METHOD_0
        write8bit(16 * type_id + method_id, &ret); // 4 bits each

        // TODO: make sure this also work for (*block)[i].size() > 1

        // TODO: simplify this by having GABAC itself have option for u7(v) num_encoded_symbols

        std::vector<uint8_t> tmp;
        insert(&(*block)[i], &tmp);
        // now get num_output_symbols from tmp:
        uint32_t num_output_symbols;
        if (tmp.size() > 0)
            num_output_symbols = 256 * (256 * (256 * tmp[0] + tmp[1]) + tmp[2]) + tmp[3];
        else
            num_output_symbols = 0;
        writeu7v(num_output_symbols, &ret);
        auto start_it = tmp.begin();
        std::advance(start_it, 4);
        ret.insert(ret.end(), start_it, tmp.end());
    }
    return ret;
}

bool descriptorEmpty(const std::vector<std::vector<gabac::DataBlock>> &data) {
    for (auto &s : data) {
        if (!s.empty())
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------------------------------------------------

AccessUnit createQuickAccessUnit(uint32_t access_unit_id, uint8_t parameter_set_id, uint32_t reads_count,
                                 DataUnit::AuType autype, DataUnit::DatasetType datatype,
                                 std::vector<std::vector<std::vector<gabac::DataBlock>>> *data,
                                 uint32_t records_count) {
    AccessUnit au(access_unit_id, parameter_set_id, autype, reads_count,
                  datatype, 32, 32, 0);
    for (size_t descriptor = 0; descriptor < data->size(); ++descriptor) {
        auto &desc = (*data)[descriptor];
        if (descriptorEmpty(desc)) {
            continue;
        }
        std::vector<uint8_t> payload;
        if (descriptor == 11 || descriptor == 15) { // msar, rname
            payload = create_payload_tokentype(&desc, records_count);
        } else {
            payload = create_payload(&desc);
        }
        au.addBlock(std::unique_ptr<Block>(new Block(descriptor, &payload)));
    }
    return au;
}

}  // namespace format
