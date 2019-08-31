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
    if(_coding_subsym_size < _output_symbol_size) {
        share_subsym_ctx_flag.push_back(_share_subsym_ctx_flag);
    }
}
Cabac_context_parameters::Cabac_context_parameters() : Cabac_context_parameters(false, 8, 8, false){

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
        const Binarization_ID& _binarization_ID,
        const Cabac_binarization_parameters& _cabac_binarization_parameters
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

void Cabac_binarization::setContextParameters(const Cabac_context_parameters& _cabac_context_parameters){
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

Descriptor_subsequence_cfg::Descriptor_subsequence_cfg(uint16_t _descriptor_subsequence_ID,
                                                       const Transform_subseq_parameters &_transform_subseq_parameters
) : descriptor_subsequence_ID(_descriptor_subsequence_ID),
    transform_subseq_parameters(_transform_subseq_parameters),
    transformSubseq_cfgs(0) {

}

void Descriptor_subsequence_cfg::addTransformSubseqCfg(const TransformSubseq_cfg &_transformSubseq_cfg) {
    transformSubseq_cfgs.push_back(_transformSubseq_cfg);
}

void Descriptor_subsequence_cfg::write(BitWriter *writer) const {
    writer->write(descriptor_subsequence_ID, 10);
    transform_subseq_parameters.write(writer);
    for (auto &i : transformSubseq_cfgs) {
        i.write(writer);
    }
}

Decoder_configuration_cabac::Decoder_configuration_cabac() : num_descriptor_subsequence_cfgs_minus1(-1),
                                                             descriptor_subsequence_cfgs(0) {

}

void Decoder_configuration_cabac::addSubsequenceCfg(const Descriptor_subsequence_cfg &cfg) {
    num_descriptor_subsequence_cfgs_minus1 += 1;
    descriptor_subsequence_cfgs.push_back(cfg);
}

void Decoder_configuration_cabac::write(BitWriter *writer) const {
    writer->write(num_descriptor_subsequence_cfgs_minus1, 8);
    for (auto &i : descriptor_subsequence_cfgs) {
        i.write(writer);
    }
}

void Decoder_configuration_tokentype::write(BitWriter *writer) const {
    transform_subseq_parameters.write(writer);
    for (auto &i : transformSubseq_cfg) {
        i.write(writer);
    }
}

Descriptor_configuration::Descriptor_configuration() : dec_cfg_preset(0),
                                                       encoding_mode_ID(0),
                                                       decoder_configuration(0),
                                                       decoder_configuration_tokentype(0) {
    if(dec_cfg_preset == 0) {
        encoding_mode_ID.push_back(0);
    }
}

void Descriptor_configuration::set_decoder_configuration(const Decoder_configuration_cabac &conf) {
    decoder_configuration.resize(1);
    decoder_configuration[0] = conf;
    decoder_configuration_tokentype.clear();
}

void Descriptor_configuration::set_decoder_configuration_tokentype(const Decoder_configuration_tokentype &conf) {
  /*  decoder_configuration_tokentype.resize(1);
    decoder_configuration_tokentype[0] = conf;
    decoder_configuration.clear();*/
    encoding_mode_ID.clear();
    dec_cfg_preset = 255; // TODO: implement token type, don't rely on reserved values
}

void Descriptor_configuration::write(BitWriter *writer) const {
    writer->write(dec_cfg_preset, 8);
    for (auto &i : encoding_mode_ID) {
        writer->write(i, 8);
    }
    for (auto &i : decoder_configuration) {
        i.write(writer);
    }
    for (auto &i : decoder_configuration_tokentype) {
        i.write(writer);
    }
}

Descriptor::Descriptor() : Descriptor(false, 1) {}

Descriptor::Descriptor(bool _class_specific_dec_cfg_flag, uint8_t _num_classes) : num_classes(_num_classes),
                                                                                  class_specific_dec_cfg_flag(
                                                                                          _class_specific_dec_cfg_flag),
                                                                                  descriptor_configurations(0) {
    if (class_specific_dec_cfg_flag) {
        GENIE_THROW_RUNTIME_EXCEPTION("Class specific config not supported");
    } else {
        descriptor_configurations.resize(1);
    }
}

void Descriptor::setConfig(const Descriptor_configuration &conf, uint8_t index) {
    descriptor_configurations[index] = conf;
}

void Descriptor::write(BitWriter *writer) const {
    writer->write(class_specific_dec_cfg_flag, 1);
    for (auto &i : descriptor_configurations) {
        i.write(writer);
    }
}

Qv_codebook::Qv_codebook() : qv_num_codebook_entries(0), qv_recon(0) {

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
}

Parameter_set_qvps::Parameter_set_qvps() : qv_num_codebooks_total(0), qv_codebooks(0) {
}

void Parameter_set_qvps::addCodeBook(const Qv_codebook &book) {
    qv_codebooks.push_back(book);
    qv_num_codebooks_total += 1;
}

void Parameter_set_qvps::write(BitWriter *writer) const {
    writer->write(qv_num_codebooks_total, 4);
    for (auto &i : qv_codebooks) {
        i.write(writer);
    }
}

Qv_coding_config::Qv_coding_config() : Qv_coding_config(Qvps_preset_ID::ASCII) {
}

Qv_coding_config::Qv_coding_config(Qvps_preset_ID _qvps_preset_ID) : qvps_flag(false), parameter_set_qvps(0),
                                                                     qvps_preset_ID(1) {
    qvps_preset_ID[0] = _qvps_preset_ID;
}

void Qv_coding_config::addQvps(const Parameter_set_qvps &_parameter_set_qvps) {
    if (qvps_preset_ID.empty())
        return;
    qvps_preset_ID.clear();
    parameter_set_qvps.push_back(_parameter_set_qvps);
}

void Qv_coding_config::write(BitWriter *writer) const {
    writer->write(qvps_flag, 1);
    for (auto &i : parameter_set_qvps) {
        i.write(writer);
    }
    for (auto &i : qvps_preset_ID) {
        writer->write(uint8_t(i), 4);
    }
}

Class_config::Class_config() : Class_config(false, Qv_coding_config()) {}

Class_config::Class_config(
        bool _qv_reverse_flag,
        const Qv_coding_config &_qv_coding_config
) : qv_coding_mode(1), qv_coding_config(0), qv_reverse_flag(_qv_reverse_flag) {
    if (qv_coding_mode) {
        qv_coding_config.push_back(_qv_coding_config);
    }
}

void Class_config::write(BitWriter *writer) const {
    writer->write(qv_coding_mode, 4);
    for (auto &i : qv_coding_config) {
        i.write(writer);
    }
    writer->write(qv_reverse_flag, 1);
}

void Cr_info::write(BitWriter &writer) {
    writer.write(cr_pad_size, 8);
    writer.write(cr_buf_max_size, 24);
}

void Parameter_set_crps::write(BitWriter &writer) {
    writer.write(uint8_t(cr_alg_ID), 8);
    for (auto &i : cr_info) {
        i.write(writer);
    }
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
    u_signature_size(0),
    class_configs(0),
    crps_flag(false),
    parameter_set_crps(0),
    internalBitCounter(0) {
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
        i.write(writer);
    }
    writer->write(num_groups, 16);
    for (auto &i : rgroup_IDs) {
        for (auto &j : i) {
            writer->write(j, 8);
        }
        writer->write(0, 8); // NULL termination
    }
    writer->write(multiple_alignments_flag, 1);
    writer->write(spliced_reads_flag, 1);
    writer->write(multiple_signature_base, 31);
    for (auto &i : u_signature_size) {
        writer->write(i, 6);
    }
    for (auto &i : class_configs) {
        i.write(writer);
    }
    writer->write(crps_flag, 1);
    for (auto &i : parameter_set_crps) {
        i.write(*writer);
    }
    writer->flush();
}

void Parameter_set::addCrps(const Parameter_set_crps &_parameter_set_crps) {
    (void) _parameter_set_crps;
    GENIE_THROW_RUNTIME_EXCEPTION("crps not supported");
}

void Parameter_set::addClass(const AU_type class_id, const Class_config &conf) {
    num_classes += 1;
    class_IDs.push_back(class_id);
    addSize(4);
    class_configs.push_back(conf);

    std::stringstream s;
    BitWriter bw(&s);
    conf.write(&bw);
    addSize(bw.getBitsWritten());
}

void Parameter_set::setDescriptor(uint8_t index, const Descriptor &descriptor) {
    std::stringstream s;
    BitWriter bw(&s);
    descriptors[index].write(&bw);
    internalBitCounter -= bw.getBitsWritten();

    descriptors[index] = descriptor;
    BitWriter bw2(&s);
    descriptors[index].write(&bw2);
    addSize(bw2.getBitsWritten());
}

void Parameter_set::addGroup(const std::string &_rgroup_id) {
    num_groups += 1;
    rgroup_IDs.push_back(_rgroup_id);
    addSize((_rgroup_id.length() + 1) * 8);
}

void Parameter_set::addMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size) {
    multiple_signature_base = _multiple_signature_base;
    u_signature_size.push_back(_U_signature_size);
    addSize(6);
}

void Parameter_set::addSize(uint64_t bits) {
    internalBitCounter += bits;
    data_unit_size = internalBitCounter / 8;
    if (internalBitCounter % 8)
        data_unit_size += 1;
}

Descriptor_subsequence_cfg
subseqFromGabac(const gabac::EncodingConfiguration &conf, uint8_t descriptorIndex, uint8_t subsequenceIndex) {
    Transform_subseq_parameters transformParams(Transform_ID_subseq(conf.sequenceTransformationId),
                                                conf.sequenceTransformationParameter);
    Descriptor_subsequence_cfg sub_conf(subsequenceIndex, transformParams);
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
        TransformSubseq_cfg subcfg(transform, Support_values(8, 8, 0, transform), binarization); // TODO insert actual values
        sub_conf.addTransformSubseqCfg(subcfg);
    }
    return sub_conf;
}

Parameter_set create_quick_parameter_set(uint8_t _parameter_set_id, uint8_t _read_length, bool paired_end, bool qv_values_present, const std::vector<std::vector<gabac::EncodingConfiguration>>& parameters) {
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
    ret.addClass(Data_unit::AU_type::U_TYPE_AU, Class_config(false, Qv_coding_config(Qv_coding_config::Qvps_preset_ID::ASCII)));
    for(int desc = 0; desc < 18; ++desc) {

        Decoder_configuration_cabac dcg;
        for(int subseq = 0; subseq < parameters[desc].size(); ++subseq) {
            dcg.addSubsequenceCfg(subseqFromGabac(parameters[desc][subseq], desc, subseq));
        }
        Descriptor_configuration dc;
        if(desc != 11 && desc != 15) {
            dc.set_decoder_configuration(dcg);
        } else {
            dc.set_decoder_configuration_tokentype(Decoder_configuration_tokentype());
        }
        Descriptor d(false, 1);
        d.setConfig(dc, 0);
        ret.setDescriptor(desc,d);
    }
    return ret;
}