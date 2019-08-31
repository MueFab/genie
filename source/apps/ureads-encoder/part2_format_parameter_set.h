#ifndef GENIE_PART2_FORMAT_PARAMETER_SET_H
#define GENIE_PART2_FORMAT_PARAMETER_SET_H

#include <cstdint>
#include <vector>
#include <gabac/gabac.h>
#include "part2_format_data_unit.h"

enum class Transform_ID_subsym : uint8_t {
    no_transform = 0,
    lut_transform = 1,
    diff_coding = 2
};

enum class Binarization_ID : uint8_t {
    Binary_Coding = 0,
    Truncated_Unary = 1,
    Exponential_Golomb = 2,
    Signed_Exponential_Gomb = 3,
    Truncated_Exponential_Golomb = 4,
    Signed_Truncated_Exponential_Golomb = 5,
    Split_Unit_wise_Truncated_Unary = 6,
    Signed_Split_Unit_wise_Truncated_Unary = 7,
    Double_Truncated_Unary = 8,
    Signed_Double_Truncated_Unary = 9,
};

enum class Transform_ID_subseq : uint8_t {
    no_transform = 0,
    equality_coding = 1,
    match_coding = 2,
    rle_coding = 3,
    merge_coding = 4
};

class Transform_subseq_parameters {
public:

    virtual void write(BitWriter *writer) const;

    Transform_subseq_parameters();

    Transform_subseq_parameters(const Transform_ID_subseq &_transform_ID_subseq, uint16_t param);

private:
    Transform_ID_subseq transform_ID_subseq; // : 8
    std::vector<uint16_t> match_coding_buffer_size; // : 16
    std::vector<uint8_t> rle_coding_guard; // : 8
    std::vector<uint8_t> merge_coding_subseq_count; // : 4
    std::vector<uint8_t> merge_coding_shift_size; // : 5
};

class Support_values {
private:
    uint8_t output_symbol_size : 6;
    uint8_t coding_subsym_size : 6;
    uint8_t coding_order : 2;
    std::vector<uint8_t> share_subsym_lut_flag; // : 1
    std::vector<uint8_t> share_subsym_prv_flag; // : 1
public:
    Support_values(uint8_t _output_symbol_size,
                   uint8_t _coding_subsym_size,
                   uint8_t _coding_order,
                   const Transform_ID_subsym& _transform_ID_subsym,
                   bool _share_subsym_prv_flag = true,
                   bool _share_subsym_lut_flag = true);
    Support_values();
    virtual void write(BitWriter *writer) const;
};

class Cabac_binarization_parameters {
private:
    std::vector<uint8_t> cmax; // : 8
    std::vector<uint8_t> cmax_teg; // : 8
    std::vector<uint8_t> cmax_dtu; // : 8
    std::vector<uint8_t> split_unit_size; // : 4
public:
    virtual void write(BitWriter *writer) const;
    Cabac_binarization_parameters();
    Cabac_binarization_parameters(const Binarization_ID& _binarization_id, uint8_t param);
};

class Cabac_context_parameters {
private:
    uint8_t adaptive_mode_flag : 1;
    uint16_t num_contexts : 16;
    std::vector<uint8_t> context_initialization_value; // : 7;
    std::vector<uint8_t> share_subsym_ctx_flag; // : 1;
public:
    Cabac_context_parameters(bool adaptive_mode_flag, uint8_t coding_subsym_size, uint8_t output_symbol_size, bool _share_subsym_ctx_flag);
    Cabac_context_parameters();
    void addContextInitializationValue(uint8_t _context_initialization_value);
    virtual void write(BitWriter *writer) const;
};

class Cabac_binarization {
private:
    Binarization_ID binarization_ID; // : 5;
    uint8_t bypass_flag : 1;
    Cabac_binarization_parameters cabac_binarization_parameters;
    std::vector<Cabac_context_parameters> cabac_context_parameters;
public:
    Cabac_binarization(const Binarization_ID& _binarization_ID, const Cabac_binarization_parameters& _cabac_binarization_parameters);
    Cabac_binarization();
    void setContextParameters(const Cabac_context_parameters& _cabac_context_parameters);
    virtual void write(BitWriter *writer) const;
};

class TransformSubseq_cfg {
public:

    TransformSubseq_cfg();

    TransformSubseq_cfg(const Transform_ID_subsym &_transform_ID_subsym, const Support_values &_support_values,
                        const Cabac_binarization &_cabac_binarization);

    virtual void write(BitWriter *writer) const;

private:
    Transform_ID_subsym transform_ID_subsym; // : 3
    Support_values support_values;
    Cabac_binarization cabac_binarization;
};

class Descriptor_subsequence_cfg {
private:
    uint16_t descriptor_subsequence_ID : 10;
    Transform_subseq_parameters transform_subseq_parameters;
    std::vector<TransformSubseq_cfg> transformSubseq_cfgs;

public:
    Descriptor_subsequence_cfg();

    Descriptor_subsequence_cfg(uint16_t descriptor_subsequence_ID,
                               const Transform_subseq_parameters &_transform_subseq_parameters);

    void addTransformSubseqCfg(const TransformSubseq_cfg &_transformSubseq_cfg);

    virtual void write(BitWriter *writer) const;
};

class Decoder_configuration_cabac {
private:
    uint8_t num_descriptor_subsequence_cfgs_minus1 : 8;
    std::vector<Descriptor_subsequence_cfg> descriptor_subsequence_cfgs;
public:
    Decoder_configuration_cabac();

    void addSubsequenceCfg(const Descriptor_subsequence_cfg &cfg);

    virtual void write(BitWriter *writer) const;
};

struct Decoder_configuration_tokentype {
    Transform_subseq_parameters transform_subseq_parameters;
    std::vector<TransformSubseq_cfg> transformSubseq_cfg;

    virtual void write(BitWriter *writer) const;
};

class Descriptor_configuration {
private:
    uint8_t dec_cfg_preset : 8;
    std::vector<uint8_t> encoding_mode_ID; // : 8
    std::vector<Decoder_configuration_cabac> decoder_configuration;
    std::vector<Decoder_configuration_tokentype> decoder_configuration_tokentype;

public:
    Descriptor_configuration();

    void set_decoder_configuration(const Decoder_configuration_cabac &conf);

    void set_decoder_configuration_tokentype(const Decoder_configuration_tokentype &conf);

    virtual void write(BitWriter *writer) const;
};

class Descriptor {
public:
    Descriptor();

    Descriptor(bool class_specific_dec_cfg_flag, uint8_t num_classes);

    void setConfig(const Descriptor_configuration &conf, uint8_t index);

    virtual void write(BitWriter *writer) const;

private:
    uint8_t num_classes;
    uint8_t class_specific_dec_cfg_flag : 1;
    std::vector<Descriptor_configuration> descriptor_configurations;
};

class Qv_codebook {
private:
    uint8_t qv_num_codebook_entries : 8;
    std::vector<uint8_t> qv_recon;

public:
    Qv_codebook();

    void addEntry(uint8_t entry);

    virtual void write(BitWriter *writer) const;
};

class Parameter_set_qvps {
private:
    uint8_t qv_num_codebooks_total : 4;
    std::vector<Qv_codebook> qv_codebooks;

public:
    Parameter_set_qvps();

    void addCodeBook(const Qv_codebook &book);

    virtual void write(BitWriter *writer) const;
};

class Qv_coding_config {
public:
    enum class Qvps_preset_ID {
        ASCII = 0,
        OFFSET33_RANGE41 = 1,
        OFFSET64_RANGE40 = 2
    };

    Qv_coding_config();

    explicit Qv_coding_config(Qvps_preset_ID _qvps_preset_ID);

    void addQvps(const Parameter_set_qvps &_parameter_set_qvps);

    virtual void write(BitWriter *writer) const;

private:
    uint8_t qvps_flag : 1;
    std::vector<Parameter_set_qvps> parameter_set_qvps;
    std::vector<Qvps_preset_ID> qvps_preset_ID; // : 4
};

class Class_config {
public:
    Class_config();

    Class_config(bool _qv_reverse_flag, const Qv_coding_config &_qv_coding_config);

    virtual void write(BitWriter *writer) const;

private:
    uint8_t qv_coding_mode : 4;
    std::vector<Qv_coding_config> qv_coding_config;
    uint8_t qv_reverse_flag : 1;
};

struct Cr_info {
    uint8_t cr_pad_size : 8;
    uint32_t cr_buf_max_size : 24;

    virtual void write(BitWriter &writer);
};

struct Parameter_set_crps {
    enum class Cr_alg_ID : uint8_t {
        reserved = 0,
        RefTransform = 1,
        PushIn = 2,
        Local_Assembly = 3,
        Global_Assembly = 4
    };

    Cr_alg_ID cr_alg_ID;
    std::vector<Cr_info> cr_info;

    virtual void write(BitWriter &writer);
};

struct Parameter_set : public Data_unit {
public:
    enum class AlphabetID : uint8_t {
        ACGTN = 0,
        ACGTRYSWKMBDHVN_ = 1
    };

    Parameter_set(
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
    );

    void addCrps(const Parameter_set_crps &parameter_set_crps);

    void addClass(const AU_type class_id, const Class_config &conf);

    void setDescriptor(uint8_t index, const Descriptor &descriptor);

    void addGroup(const std::string &_rgroup_id);

    void addMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size);

    void write(BitWriter *writer) override;

private:
    uint16_t reserved : 10;
    uint32_t data_unit_size : 22;

    // encoding_parameters
    uint8_t parameter_set_ID : 8;
    uint8_t parent_parameter_set_ID : 8;
    Dataset_type dataset_type; // : 4
    AlphabetID alphabet_ID : 8;
    uint32_t read_length : 24;
    uint8_t number_of_template_segments_minus1 : 2;
    uint8_t reserved_2 : 6;
    uint32_t max_au_data_unit_size : 29;
    uint8_t pos_40_bits_flag : 1;
    uint8_t qv_depth : 3;
    uint8_t as_depth : 3;
    uint8_t num_classes : 4;
    std::vector<AU_type> class_IDs; // : 4
    std::vector<Descriptor> descriptors;
    uint16_t num_groups : 16;
    std::vector<std::string> rgroup_IDs;
    uint8_t multiple_alignments_flag : 1;
    uint8_t spliced_reads_flag : 1;
    uint32_t multiple_signature_base : 31;
    std::vector<uint8_t> u_signature_size; // : 6
    std::vector<Class_config> class_configs;
    uint8_t crps_flag : 1;
    std::vector<Parameter_set_crps> parameter_set_crps;

    uint64_t internalBitCounter;

    void addSize(uint64_t bits);
};

Parameter_set create_quick_parameter_set(uint8_t _parameter_set_id,
        uint8_t _read_length,
        bool paired_end,
        bool qv_values_present,
        const std::vector<std::vector<gabac::EncodingConfiguration>>& parameters
);

#endif //GENIE_PART2_FORMAT_PARAMETER_SET_H
