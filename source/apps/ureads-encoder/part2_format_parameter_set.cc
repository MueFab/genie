#include "part2_format_parameter_set.h"
#include "exceptions.h"

#include <sstream>

Transform_subseq_parameters::Transform_subseq_parameters() : Transform_subseq_parameters(
        Transform_ID_subseq::no_transform, 0) {

}

Transform_subseq_parameters::Transform_subseq_parameters(
        const Transform_ID_subseq &_transform_ID_subseq,
        uint16_t param
) : transform_ID_subseq(_transform_ID_subseq),
    match_coding_buffer_size(0),
    rle_coding_guard(0),
    merge_coding_subseq_count(0),
    merge_coding_shift_size(0) {

    switch (transform_ID_subseq) {
        case Transform_ID_subseq::no_transform:
        case Transform_ID_subseq::equality_coding:
            break;
        case Transform_ID_subseq::rle_coding:
            rle_coding_guard.push_back(param);
            break;
        case Transform_ID_subseq::match_coding:
            match_coding_buffer_size.push_back(param);
            break;
        case Transform_ID_subseq::merge_coding:
            GENIE_THROW_RUNTIME_EXCEPTION("Merge coding not supported");
            break;
    }
}

void Transform_subseq_parameters::write(BitWriter *writer) const {
    writer->write(uint8_t(transform_ID_subseq), 8);
    for (auto &i : match_coding_buffer_size) {
        writer->write(i, 16);
    }
    for (auto &i : rle_coding_guard) {
        writer->write(i, 8);
    }
    for (auto &i : merge_coding_subseq_count) {
        writer->write(i, 4);
    }
    for (auto &i : merge_coding_shift_size) {
        writer->write(i, 5);
    }
}

Support_values::Support_values(uint8_t _output_symbol_size,
                               uint8_t _coding_subsym_size,
                               uint8_t _coding_order,
                               const Transform_ID_subsym &transform_ID_subsym,
                               bool _share_subsym_prv_flag,
                               bool _share_subsym_lut_flag)
        : output_symbol_size(_output_symbol_size),
          coding_subsym_size(_coding_subsym_size),
          coding_order(_coding_order),
          share_subsym_lut_flag(0),
          share_subsym_prv_flag(0) {

    if (coding_subsym_size < output_symbol_size && coding_order > 0) {
        if (transform_ID_subsym == Transform_ID_subsym::lut_transform) {
            share_subsym_lut_flag.push_back(_share_subsym_lut_flag);
        }
        share_subsym_prv_flag.push_back(_share_subsym_prv_flag);
    }
}

Support_values::Support_values() : Support_values(0, 0, 0, Transform_ID_subsym::no_transform) {

}

void Support_values::write(BitWriter *writer) const {
    writer->write(output_symbol_size, 6);
    writer->write(coding_subsym_size, 6);
    writer->write(coding_order, 2);
    for (auto &i : share_subsym_lut_flag) {
        writer->write(i, 1);
    }
    for (auto &i : share_subsym_prv_flag) {
        writer->write(i, 1);
    }
}

void Cabac_binarization_parameters::write(BitWriter *writer) const {
    for (auto &i : cmax) {
        writer->write(i, 8);
    }
    for (auto &i : cmax_teg) {
        writer->write(i, 8);
    }
    for (auto &i : cmax_dtu) {
        writer->write(i, 8);
    }
    for (auto &i : split_unit_size) {
        writer->write(i, 4);
    }
}

Cabac_binarization_parameters::Cabac_binarization_parameters() : Cabac_binarization_parameters(
        Binarization_ID::Binary_Coding, 0) {

}

Cabac_binarization_parameters::Cabac_binarization_parameters(
        const Binarization_ID &_binarization_id,
        uint8_t param
) : cmax(0),
    cmax_teg(0),
    cmax_dtu(0),
    split_unit_size(0) {

    switch (_binarization_id) {
        case Binarization_ID::Truncated_Unary:
            cmax.push_back(param);
            break;
        case Binarization_ID::Truncated_Exponential_Golomb:
        case Binarization_ID::Signed_Truncated_Exponential_Golomb:
            cmax_teg.push_back(param);
            break;
        case Binarization_ID::Binary_Coding:
        case Binarization_ID::Exponential_Golomb:
        case Binarization_ID::Signed_Exponential_Gomb:
            break;
        default:
            GENIE_THROW_RUNTIME_EXCEPTION("Binarization not supported");
    }

}

Cabac_context_parameters::Cabac_context_parameters(
        bool _adaptive_mode_flag,
        uint8_t _coding_subsym_size,
        uint8_t _output_symbol_size,
        bool _share_subsym_ctx_flag
) : adaptive_mode_flag(_adaptive_mode_flag),
    num_contexts(0),
    context_initialization_value(0),
    share_subsym_ctx_flag(0) {
    if (_coding_subsym_size < _output_symbol_size) {
        share_subsym_ctx_flag.push_back(_share_subsym_ctx_flag);
    }
}

Cabac_context_parameters::Cabac_context_parameters() : Cabac_context_parameters(false, 8, 8, false) {

}

void Cabac_context_parameters::addContextInitializationValue(uint8_t _context_initialization_value) {
    ++num_contexts;
    share_subsym_ctx_flag.push_back(_context_initialization_value);
}

void Cabac_context_parameters::write(BitWriter *writer) const {
    writer->write(adaptive_mode_flag, 1);
    writer->write(num_contexts, 16);
    for (auto &i : context_initialization_value) {
        writer->write(i, 7);
    }
    for (auto &i : share_subsym_ctx_flag) {
        writer->write(i, 1);
    }
}

Cabac_binarization::Cabac_binarization(
        const Binarization_ID &_binarization_ID,
        const Cabac_binarization_parameters &_cabac_binarization_parameters
) : binarization_ID(_binarization_ID),
    bypass_flag(true),
    cabac_binarization_parameters(_cabac_binarization_parameters),
    cabac_context_parameters(0) {
}

Cabac_binarization::Cabac_binarization() : Cabac_binarization(
        Binarization_ID::Binary_Coding,
        Cabac_binarization_parameters(Binarization_ID::Binary_Coding, 0)
) {

}

void Cabac_binarization::setContextParameters(const Cabac_context_parameters &_cabac_context_parameters) {
    bypass_flag = false;
    cabac_context_parameters.push_back(_cabac_context_parameters);
}

void Cabac_binarization::write(BitWriter *writer) const {
    writer->write(uint8_t(binarization_ID), 5);
    writer->write(bypass_flag, 1);
    cabac_binarization_parameters.write(writer);
    for (auto &i : cabac_context_parameters) {
        i.write(writer);
    }
}

TransformSubseq_cfg::TransformSubseq_cfg() : TransformSubseq_cfg(Transform_ID_subsym::no_transform, Support_values(),
                                                                 Cabac_binarization()) {

}

TransformSubseq_cfg::TransformSubseq_cfg(const Transform_ID_subsym &_transform_ID_subsym,
                                         const Support_values &_support_values,
                                         const Cabac_binarization &_cabac_binarization)
        : transform_ID_subsym(_transform_ID_subsym),
          support_values(_support_values),
          cabac_binarization(_cabac_binarization) {
}

void TransformSubseq_cfg::write(BitWriter *writer) const {
    writer->write(uint8_t(transform_ID_subsym), 3);
    support_values.write(writer);
    cabac_binarization.write(writer);
}

Descriptor_subsequence_cfg::Descriptor_subsequence_cfg() {

}

Descriptor_subsequence_cfg::Descriptor_subsequence_cfg(
        std::unique_ptr<Transform_subseq_parameters> _transform_subseq_parameters,
        uint16_t _descriptor_subsequence_ID
) : descriptor_subsequence_ID(_descriptor_subsequence_ID),
    transform_subseq_parameters(std::move(_transform_subseq_parameters)),
    transformSubseq_cfgs(0) {

}

void Descriptor_subsequence_cfg::addTransformSubseqCfg(std::unique_ptr<TransformSubseq_cfg> _transformSubseq_cfg) {
    transformSubseq_cfgs.push_back(std::move(_transformSubseq_cfg));
}

void Descriptor_subsequence_cfg::write(BitWriter *writer) const {
    writer->write(descriptor_subsequence_ID, 10);
    transform_subseq_parameters->write(writer);
    for (auto &i : transformSubseq_cfgs) {
        i->write(writer);
    }
}

Decoder_configuration_cabac_regular::Decoder_configuration_cabac_regular() : num_descriptor_subsequence_cfgs_minus1(-1),
                                                                             descriptor_subsequence_cfgs(0) {

}

void Decoder_configuration_cabac_regular::addSubsequenceCfg(std::unique_ptr<Descriptor_subsequence_cfg> cfg) {
    num_descriptor_subsequence_cfgs_minus1 += 1;
    descriptor_subsequence_cfgs.push_back(std::move(cfg));
}

void Decoder_configuration_cabac_regular::write(BitWriter *writer) const {
    Decoder_configuration_cabac::write(writer);
    writer->write(num_descriptor_subsequence_cfgs_minus1, 8);
    for (auto &i : descriptor_subsequence_cfgs) {
        i->write(writer);
    }
}

/*void Decoder_configuration_cabac_tokentype::write(BitWriter *writer) const {
    Decoder_configuration_cabac::write(writer);
    transform_subseq_parameters.write(writer);
    for (auto &i : transformSubseq_cfg) {
        i.write(writer);
    }
}*/

void Descriptor_configuration_present::set_decoder_configuration(std::unique_ptr<Decoder_configuration> conf) {
    decoder_configuration = std::move(conf);
}

void Descriptor_configuration_present::_deactivate() {
    /*  decoder_configuration_tokentype.resize(1);
      decoder_configuration_tokentype[0] = conf;
      decoder_configuration.clear();*/
    dec_cfg_preset = Dec_cfg_preset(255); // TODO: implement token type, don't rely on reserved values
}

void Descriptor_configuration::write(BitWriter *writer) const {
    writer->write(uint8_t(dec_cfg_preset), 8);
}

Descriptor::Descriptor() : class_specific_dec_cfg_flag(false), descriptor_configurations(0) {
    descriptor_configurations.push_back(make_unique<Descriptor_configuration_present>());
}

void Descriptor::setConfig(uint8_t index, std::unique_ptr<Descriptor_configuration> conf) {
    if (index > descriptor_configurations.size()) {
        GENIE_THROW_RUNTIME_EXCEPTION("Config index out of bounds.");
    }
    descriptor_configurations[index] = std::move(conf);
}

void Descriptor::setConfig(std::unique_ptr<Descriptor_configuration> conf) {
    if (class_specific_dec_cfg_flag) {
        GENIE_THROW_RUNTIME_EXCEPTION("Invalid setConfig() for non class-specific descriptor config.");
    }
    descriptor_configurations[0] = std::move(conf);
}

void Descriptor::enableClassSpecificConfigs(uint8_t numClasses) {
    if (class_specific_dec_cfg_flag) {
        GENIE_THROW_RUNTIME_EXCEPTION("Class specific configs already enabled");
    }
    class_specific_dec_cfg_flag = true;
    descriptor_configurations.resize(numClasses);
    for (size_t i = 1; i < descriptor_configurations.size(); ++i) {
        descriptor_configurations[i] = descriptor_configurations[0]->clone();
    }
}

void Descriptor::write(BitWriter *writer) const {
    writer->write(class_specific_dec_cfg_flag, 1);
    for (auto &i : descriptor_configurations) {
        i->write(writer);
    }
}

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

Qv_coding_config::Qv_coding_config(Qv_coding_mode _qv_coding_mode, bool _qv_reverse_flag) : qv_coding_mode(
        _qv_coding_mode), qv_reverse_flag(_qv_reverse_flag) {
}

Qv_coding_config_1::Qv_coding_config_1() : Qv_coding_config_1(Qvps_preset_ID::ASCII, false) {
}

Qv_coding_config_1::Qv_coding_config_1(Qvps_preset_ID _qvps_preset_ID, bool _reverse_flag) : Qv_coding_config(
        Qv_coding_mode::ONE, _reverse_flag), qvps_flag(false), parameter_set_qvps(nullptr) {
    qvps_preset_ID = make_unique<Qvps_preset_ID>(_qvps_preset_ID);
}

void Qv_coding_config_1::setQvps(std::unique_ptr<Parameter_set_qvps> _parameter_set_qvps) {
    qvps_flag = true;
    qvps_preset_ID = nullptr;
    parameter_set_qvps = std::move(_parameter_set_qvps);
}

void Qv_coding_config_1::write(BitWriter *writer) const {
    writer->write(qvps_flag, 1);
    if (parameter_set_qvps) {
        parameter_set_qvps->write(writer);
    }
    if (qvps_preset_ID) {
        writer->write(uint64_t(*qvps_preset_ID), 4);
    }
    writer->write(qv_reverse_flag, 1);
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


Parameter_set::Parameter_set(
        uint8_t _parameter_set_ID,
        uint8_t _parent_parameter_set_ID,
        Dataset_type _dataset_type,
        AlphabetID _alphabet_id,
        uint32_t _read_length,
        bool _paired_end,
        bool _pos_40_bits_flag,
        uint8_t _qv_depth,
        uint8_t _as_depth,
        bool _multiple_alignments_flag,
        bool _spliced_reads_flag
) : Data_unit(Data_unit_type::parameter_set),
    reserved(0),
    data_unit_size(0),
    parameter_set_ID(_parameter_set_ID),
    parent_parameter_set_ID(_parent_parameter_set_ID),
    dataset_type(_dataset_type),
    alphabet_ID(_alphabet_id),
    read_length(_read_length),
    number_of_template_segments_minus1(_paired_end),
    reserved_2(0),
    max_au_data_unit_size(0),
    pos_40_bits_flag(_pos_40_bits_flag),
    qv_depth(_qv_depth),
    as_depth(_as_depth),
    num_classes(0),
    class_IDs(0),
    descriptors(18),
    num_groups(0),
    rgroup_IDs(0),
    multiple_alignments_flag(_multiple_alignments_flag),
    spliced_reads_flag(_spliced_reads_flag),
    multiple_signature_base(0),
    u_signature_size(nullptr),
    qv_coding_configs(0),
    crps_flag(false),
    parameter_set_crps(nullptr),
    internalBitCounter(0) {
    for (auto &i : descriptors) {
        i = make_unique<Descriptor>();
    }
    std::stringstream s;
    BitWriter bw(&s);
    write(&bw);
    addSize(bw.getBitsWritten());
}

void Parameter_set::write(BitWriter *writer) {
    Data_unit::write(writer);
    writer->write(reserved, 10);
    writer->write(data_unit_size, 22);
    writer->write(parameter_set_ID, 8);
    writer->write(parent_parameter_set_ID, 8);
    writer->write(uint8_t(dataset_type), 4);
    writer->write(uint8_t(alphabet_ID), 8);
    writer->write(read_length, 24);
    writer->write(number_of_template_segments_minus1, 2);
    writer->write(reserved_2, 6);
    writer->write(max_au_data_unit_size, 29);
    writer->write(pos_40_bits_flag, 1);
    writer->write(qv_depth, 3);
    writer->write(as_depth, 3);
    writer->write(num_classes, 4);
    for (auto &i : class_IDs) {
        writer->write(uint8_t(i), 4);
    }
    for (auto &i : descriptors) {
        i->write(writer);
    }
    writer->write(num_groups, 16);
    for (auto &i : rgroup_IDs) {
        for (auto &j : *i) {
            writer->write(j, 8);
        }
        writer->write(0, 8); // NULL termination
    }
    writer->write(multiple_alignments_flag, 1);
    writer->write(spliced_reads_flag, 1);
    writer->write(multiple_signature_base, 31);
    if (u_signature_size) {
        writer->write(*u_signature_size, 6);
    }
    for (auto &i : qv_coding_configs) {
        i->write(writer);
    }
    writer->write(crps_flag, 1);
    if (parameter_set_crps) {
        parameter_set_crps->write(*writer);
    }
    writer->flush();
}

void Parameter_set::setCrps(std::unique_ptr<Parameter_set_crps> _parameter_set_crps) {
    (void) _parameter_set_crps;
    GENIE_THROW_RUNTIME_EXCEPTION("crps not supported");
}

void Parameter_set::addClass(AU_type class_id, std::unique_ptr<Qv_coding_config> conf) {
    std::stringstream s;
    BitWriter bw(&s);
    conf->write(&bw);
    addSize(bw.getBitsWritten());

    num_classes += 1;
    class_IDs.push_back(class_id);
    addSize(4);
    qv_coding_configs.push_back(std::move(conf));
}

void Parameter_set::setDescriptor(uint8_t index, std::unique_ptr<Descriptor> descriptor) {
    std::stringstream s;
    BitWriter bw(&s);
    descriptors[index]->write(&bw);
    internalBitCounter -= bw.getBitsWritten();

    descriptors[index] = std::move(descriptor);
    BitWriter bw2(&s);
    descriptors[index]->write(&bw2);
    addSize(bw2.getBitsWritten());
}

void Parameter_set::addGroup(std::unique_ptr<std::string> rgroup_id) {
    GENIE_THROW_RUNTIME_EXCEPTION("Groups not supported");
    num_groups += 1;
    rgroup_IDs.push_back(std::move(rgroup_id));
    addSize((rgroup_id->length() + 1) * 8);
}

void Parameter_set::setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size) {
    GENIE_THROW_RUNTIME_EXCEPTION("Signature base not supported");
    multiple_signature_base = _multiple_signature_base;
    u_signature_size = make_unique<uint8_t>(_U_signature_size);
    addSize(6);
}

void Parameter_set::addSize(uint64_t bits) {
    internalBitCounter += bits;
    data_unit_size = internalBitCounter / 8;
    if (internalBitCounter % 8)
        data_unit_size += 1;
}

std::unique_ptr<Descriptor_subsequence_cfg>
subseqFromGabac(const gabac::EncodingConfiguration &conf, uint8_t subsequenceIndex) {
    auto transformParams = make_unique<Transform_subseq_parameters>(Transform_ID_subseq(conf.sequenceTransformationId),
                                                conf.sequenceTransformationParameter);
    auto sub_conf = make_unique<Descriptor_subsequence_cfg>(std::move(transformParams), subsequenceIndex);
    for (const auto &i : conf.transformedSequenceConfigurations) {
        Transform_ID_subsym transform = Transform_ID_subsym::no_transform;
        if (i.lutTransformationEnabled && i.diffCodingEnabled) {
            GENIE_THROW_RUNTIME_EXCEPTION("LUT and Diff coding at the same time not supported");
        } else if (i.lutTransformationEnabled) {
            transform = Transform_ID_subsym::lut_transform;
        } else if (i.diffCodingEnabled) {
            transform = Transform_ID_subsym::diff_coding;
        }
        Cabac_binarization binarization(
                Binarization_ID(i.binarizationId),
                Cabac_binarization_parameters(
                        Binarization_ID(i.binarizationId), i.binarizationParameters[0]
                )
        );
        if (i.contextSelectionId != gabac::ContextSelectionId::bypass) {
            binarization.setContextParameters(Cabac_context_parameters(false, 8, 8, 0)); //TODO insert actual values
        }
        auto subcfg = make_unique<TransformSubseq_cfg>(transform, Support_values(8, 8, 0, transform),
                                   binarization); // TODO insert actual values
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