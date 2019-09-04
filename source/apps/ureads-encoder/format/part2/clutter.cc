#include <array>

#include "clutter.h"
#include "ureads-encoder/format/part2/qv_coding_config_1/qv_coding_config_1.h"
#include "ureads-encoder/format/part2/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h"
#include "ureads-encoder/format/part2/descriptor_configuration_present/decoder_configuration.h"
#include "ureads-encoder/format/part2/descriptor_configuration_present/cabac/decoder_configuration_cabac_regular.h"
#include "ureads-encoder/format/part2/descriptor_configuration_present/descriptor_configuration_present.h"
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
                         {"msar", 2},
                         {"rtype", 1},
                         {"rgroup", 1},
                         {"qv", 0},
                         {"rname", 2},
                         {"rftp", 1},
                         {"rftt", 1}
                 }};
        return prop;
    }

/*void Decoder_configuration_cabac_tokentype::write(BitWriter *writer) const {
    DecoderConfigurationCabac::write(writer);
    transform_subseq_parameters.write(writer);
    for (auto &i : transformSubseq_cfg) {
        i.write(writer);
    }
}*/


/*QvCodebook::QvCodebook() : qv_num_codebook_entries(0), qv_recon(0) {

}

void QvCodebook::addEntry(uint8_t entry) {
    qv_num_codebook_entries += 1;
    qv_recon.push_back(entry);
}

void QvCodebook::write(BitWriter *writer) const {
    writer->write(qv_num_codebook_entries, 8);
    for (auto &i : qv_recon) {
        writer->write(i, 8);
    }
}*/

/*ParameterSetQvps::ParameterSetQvps() : qv_num_codebooks_total(0), qv_codebooks(0) {
}

void ParameterSetQvps::addCodeBook(const QvCodebook &book) {
    qv_codebooks.push_back(book);
    qv_num_codebooks_total += 1;
}*/

    void qv_coding1::ParameterSetQvps::write(BitWriter *writer) const {
        /*  writer->write(qv_num_codebooks_total, 4);
          for (auto &i : qv_codebooks) {
              i.write(writer);
          } */
    }


    void CrInfo::write(BitWriter &writer) {
/*    writer.write(cr_pad_size, 8);
    writer.write(cr_buf_max_size, 24); */
    }

    void ParameterSetCrps::write(BitWriter &writer) {
        /*    writer.write(uint8_t(cr_alg_ID), 8);
        for (auto &i : cr_info) {
            i.write(writer);
        }*/
    }


    std::unique_ptr<desc_conf_pres::cabac::DescriptorSubsequenceCfg>
    subseqFromGabac(const gabac::EncodingConfiguration &conf, uint8_t subsequenceIndex) {
        auto transformParams = make_unique<desc_conf_pres::cabac::TransformSubseqParameters>(
                desc_conf_pres::cabac::TransformSubseqParameters::TransformIdSubseq(conf.sequenceTransformationId),
                conf.sequenceTransformationParameter);
        auto sub_conf = make_unique<desc_conf_pres::cabac::DescriptorSubsequenceCfg>(std::move(transformParams), subsequenceIndex);
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
            dcg = make_unique<desc_conf_pres::cabac::DecoderConfigurationCabacRegular>();
            for (size_t subseq = 0; subseq < parameters[desc].size(); ++subseq) {
                dcg->addSubsequenceCfg(subseqFromGabac(parameters[desc][subseq], subseq));
            }
            auto dc = make_unique<desc_conf_pres::DescriptorConfigurationPresent>();
            if (desc != 11 && desc != 15) {
                dc->set_decoder_configuration(std::move(dcg));
            } else {
                dc->_deactivate();
            }
            auto d = make_unique<DescriptorConfigurationContainer>();
            d->setConfig(std::move(dc));
            ret.setDescriptor(desc, std::move(d));
        }
        return ret;
    }

}