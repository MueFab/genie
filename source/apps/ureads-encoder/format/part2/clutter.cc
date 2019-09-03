#include <array>

#include "clutter.h"
#include "qv_coding_config_1.h"
#include "descriptor_subsequence_cfg.h"
#include "decoder_configuration.h"
#include "decoder_configuration_cabac_regular.h"
#include "descriptor_configuration_present.h"
#include "make_unique.h"


const std::array<Genomic_descriptor_properties, NUM_DESCRIPTORS> &getDescriptorProperties() {
    static std::array<Genomic_descriptor_properties, NUM_DESCRIPTORS> prop =
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
    Decoder_configuration_cabac::write(writer);
    transform_subseq_parameters.write(writer);
    for (auto &i : transformSubseq_cfg) {
        i.write(writer);
    }
}*/


/*Qv_codebook::Qv_codebook() : qv_num_codebook_entries(0), qv_recon(0) {

}

void Qv_codebook::addEntry(uint8_t entry) {
    qv_num_codebook_entries += 1;
    qv_recon.push_back(entry);
}

void Qv_codebook::write(BitWriter *writer) const {
    writer->write(qv_num_codebook_entries, 8);
    for (auto &i : qv_recon) {
        writer->write(i, 8);
    }
}*/

/*Parameter_set_qvps::Parameter_set_qvps() : qv_num_codebooks_total(0), qv_codebooks(0) {
}

void Parameter_set_qvps::addCodeBook(const Qv_codebook &book) {
    qv_codebooks.push_back(book);
    qv_num_codebooks_total += 1;
}*/

void Parameter_set_qvps::write(BitWriter *writer) const {
    /*  writer->write(qv_num_codebooks_total, 4);
      for (auto &i : qv_codebooks) {
          i.write(writer);
      } */
}


void Cr_info::write(BitWriter &writer) {
/*    writer.write(cr_pad_size, 8);
    writer.write(cr_buf_max_size, 24); */
}

void Parameter_set_crps::write(BitWriter &writer) {
    /*    writer.write(uint8_t(cr_alg_ID), 8);
    for (auto &i : cr_info) {
        i.write(writer);
    }*/
}


std::unique_ptr<Descriptor_subsequence_cfg>
subseqFromGabac(const gabac::EncodingConfiguration &conf, uint8_t subsequenceIndex) {
    auto transformParams = make_unique<Transform_subseq_parameters>(Transform_subseq_parameters::Transform_ID_subseq(conf.sequenceTransformationId),
                                                                    conf.sequenceTransformationParameter);
    auto sub_conf = make_unique<Descriptor_subsequence_cfg>(std::move(transformParams), subsequenceIndex);
    for (const auto &i : conf.transformedSequenceConfigurations) {
        Support_values::Transform_ID_subsym transform = Support_values::Transform_ID_subsym::no_transform;
        if (i.lutTransformationEnabled && i.diffCodingEnabled) {
            GENIE_THROW_RUNTIME_EXCEPTION("LUT and Diff coding at the same time not supported");
        } else if (i.lutTransformationEnabled) {
            transform = Support_values::Transform_ID_subsym::lut_transform;
        } else if (i.diffCodingEnabled) {
            transform = Support_values::Transform_ID_subsym::diff_coding;
        }
        auto binarization = make_unique<Cabac_binarization>(
                Cabac_binarization_parameters::Binarization_ID(i.binarizationId),
                make_unique<Cabac_binarization_parameters>(
                        Cabac_binarization_parameters::Binarization_ID(i.binarizationId), i.binarizationParameters[0]
                )
        );
        if (i.contextSelectionId != gabac::ContextSelectionId::bypass) {
            binarization->setContextParameters(
                    make_unique<Cabac_context_parameters>(false, 8, 8, 0)); //TODO insert actual values
        }
        auto subcfg = make_unique<Transform_subseq_cfg>(transform, make_unique<Support_values>(8, 8, 0, transform),
                                                        std::move(binarization)); // TODO insert actual values
        sub_conf->addTransformSubseqCfg(std::move(subcfg));
    }
    return sub_conf;
}

Parameter_set
create_quick_parameter_set(uint8_t _parameter_set_id, uint8_t _read_length, bool paired_end, bool qv_values_present,
                           const std::vector<std::vector<gabac::EncodingConfiguration>> &parameters) {
    Parameter_set ret(_parameter_set_id,
                      _parameter_set_id,
                      Data_unit::Dataset_type::non_aligned,
                      Parameter_set::AlphabetID::ACGTN,
                      _read_length,
                      paired_end,
                      false,
                      qv_values_present,
                      0,
                      false,
                      false
    );
    ret.addClass(Data_unit::AU_type::U_TYPE_AU,
                 make_unique<Qv_coding_config_1>(Qv_coding_config_1::Qvps_preset_ID::ASCII, false));
    for (int desc = 0; desc < 18; ++desc) {

        std::unique_ptr<Decoder_configuration> dcg;
        dcg = make_unique<Decoder_configuration_cabac_regular>();
        for (size_t subseq = 0; subseq < parameters[desc].size(); ++subseq) {
            dcg->addSubsequenceCfg(subseqFromGabac(parameters[desc][subseq], subseq));
        }
        auto dc = make_unique<Descriptor_configuration_present>();
        if (desc != 11 && desc != 15) {
            dc->set_decoder_configuration(std::move(dcg));
        } else {
            dc->_deactivate();
        }
        auto d = make_unique<Descriptor>();
        d->setConfig(std::move(dc));
        ret.setDescriptor(desc, std::move(d));
    }
    return ret;
}