#ifndef GENIE_PART2_FORMAT_PARAMETER_SET_H
#define GENIE_PART2_FORMAT_PARAMETER_SET_H

/* ----------------------------------------------------------------------------------------------------------- */

#include <cstdint>
#include <vector>
#include <memory>
#include <gabac/gabac.h>
#include "part2_format_data_unit.h"

/* ----------------------------------------------------------------------------------------------------------- */

template<class T>
struct _Unique_if {
    typedef std::unique_ptr<T> _Single_object;
};

template<class T>
struct _Unique_if<T[]> {
    typedef std::unique_ptr<T[]> _Unknown_bound;
};

template<class T, size_t N>
struct _Unique_if<T[N]> {
    typedef void _Known_bound;
};

template<class T, class... Args>
typename _Unique_if<T>::_Single_object
make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
typename _Unique_if<T>::_Unknown_bound
make_unique(size_t n) {
    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[n]());
}

template<class T, class... Args>
typename _Unique_if<T>::_Known_bound
make_unique(Args &&...) = delete;

/* ----------------------------------------------------------------------------------------------------------- */

struct Cr_info {
    /*  uint8_t cr_pad_size : 8;
      uint32_t cr_buf_max_size : 24;*/

    virtual void write(BitWriter &writer);
};

/* ----------------------------------------------------------------------------------------------------------- */

struct Parameter_set_crps {
    /*  enum class Cr_alg_ID : uint8_t {
          reserved = 0,
          RefTransform = 1,
          PushIn = 2,
          Local_Assembly = 3,
          Global_Assembly = 4
      };

      Cr_alg_ID cr_alg_ID;
      std::vector<Cr_info> cr_info;*/

    virtual void write(BitWriter &writer);
};

/* ----------------------------------------------------------------------------------------------------------- */

class Qv_codebook {
    /*
private:
    uint8_t qv_num_codebook_entries : 8;
    std::vector<uint8_t> qv_recon;

public:
    Qv_codebook();

    void addEntry(uint8_t entry);

    virtual void write(BitWriter *writer) const;*/
};

/* ----------------------------------------------------------------------------------------------------------- */

class Parameter_set_qvps {
private:
/*    uint8_t qv_num_codebooks_total : 4;
    std::vector<Qv_codebook> qv_codebooks;

public:
    Parameter_set_qvps();

    void addCodeBook(const Qv_codebook &book); */
public:
    virtual void write(BitWriter *writer) const;
};

/* ----------------------------------------------------------------------------------------------------------- */

/*class Decoder_configuration_cabac_tokentype : public Decoder_configuration_cabac {
private:
    Transform_subseq_parameters transform_subseq_parameters;
    std::vector<TransformSubseq_cfg> transformSubseq_cfg;

public:
    void addSubsequenceCfg(const Descriptor_subsequence_cfg &cfg) override {
        (void) cfg;
    }

    std::unique_ptr<Decoder_configuration> clone() const override {
        return make_unique<Decoder_configuration_cabac_tokentype>();
    }

    void write(BitWriter *writer) const override;
};*/

/* ----------------------------------------------------------------------------------------------------------- */

enum class Transform_ID_subsym : uint8_t {
    no_transform = 0,
    lut_transform = 1,
    diff_coding = 2
};

/* ----------------------------------------------------------------------------------------------------------- */

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

/* ----------------------------------------------------------------------------------------------------------- */

enum class Transform_ID_subseq : uint8_t {
    no_transform = 0,
    equality_coding = 1,
    match_coding = 2,
    rle_coding = 3,
    merge_coding = 4
};

/* ----------------------------------------------------------------------------------------------------------- */

enum class Genomic_descriptor : uint8_t {
    pos = 0,
    rcomp = 1,
    flags = 2,
    mmpos = 3,
    mmtype = 4,
    clips = 5,
    ureads = 6,
    rlen = 7,
    pair = 8,
    mscore = 9,
    mmap = 10,
    msar = 11,
    rtype = 12,
    rgroup = 13,
    qv = 14,
    rname = 15,
    rftp = 16,
    rftt = 17
};

struct Genomic_descriptor_properties {
    std::string name;
    uint8_t number_subsequences;
};

static const Genomic_descriptor_properties &getDescriptorProperties() {
    constexpr size_t NUM_DESCRIPTORS = 18;
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
}


/* ----------------------------------------------------------------------------------------------------------- */

class Transform_subseq_parameters {
public:

    virtual void write(BitWriter *writer) const;

    Transform_subseq_parameters();

    Transform_subseq_parameters(const Transform_ID_subseq &_transform_ID_subseq, uint16_t param);

    std::unique_ptr<Transform_subseq_parameters> clone () const {
        auto ret = make_unique<Transform_subseq_parameters>();
        ret->transform_ID_subseq = transform_ID_subseq;
        ret->match_coding_buffer_size = match_coding_buffer_size;
        ret->rle_coding_guard = rle_coding_guard;
        ret->merge_coding_subseq_count = merge_coding_subseq_count;
        ret->merge_coding_shift_size = merge_coding_shift_size;
        return ret;
    }

private:
    Transform_ID_subseq transform_ID_subseq; // : 8
    std::vector<uint16_t> match_coding_buffer_size; // : 16
    std::vector<uint8_t> rle_coding_guard; // : 8
    std::vector<uint8_t> merge_coding_subseq_count; // : 4
    std::vector<uint8_t> merge_coding_shift_size; // : 5
};

/* ----------------------------------------------------------------------------------------------------------- */

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
                   const Transform_ID_subsym &_transform_ID_subsym,
                   bool _share_subsym_prv_flag = true,
                   bool _share_subsym_lut_flag = true);

    Support_values();

    virtual void write(BitWriter *writer) const;
};

/* ----------------------------------------------------------------------------------------------------------- */

class Cabac_binarization_parameters {
private:
    std::vector<uint8_t> cmax; // : 8
    std::vector<uint8_t> cmax_teg; // : 8
    std::vector<uint8_t> cmax_dtu; // : 8
    std::vector<uint8_t> split_unit_size; // : 4
public:
    virtual void write(BitWriter *writer) const;

    Cabac_binarization_parameters();

    Cabac_binarization_parameters(const Binarization_ID &_binarization_id, uint8_t param);
};

/* ----------------------------------------------------------------------------------------------------------- */

class Cabac_context_parameters {
private:
    uint8_t adaptive_mode_flag : 1;
    uint16_t num_contexts : 16;
    std::vector<uint8_t> context_initialization_value; // : 7;
    std::vector<uint8_t> share_subsym_ctx_flag; // : 1;
public:
    Cabac_context_parameters(bool adaptive_mode_flag, uint8_t coding_subsym_size, uint8_t output_symbol_size,
                             bool _share_subsym_ctx_flag);

    Cabac_context_parameters();

    void addContextInitializationValue(uint8_t _context_initialization_value);

    virtual void write(BitWriter *writer) const;
};

/* ----------------------------------------------------------------------------------------------------------- */

class Cabac_binarization {
private:
    Binarization_ID binarization_ID; // : 5;
    uint8_t bypass_flag : 1;
    Cabac_binarization_parameters cabac_binarization_parameters;
    std::vector<Cabac_context_parameters> cabac_context_parameters;
public:
    Cabac_binarization(const Binarization_ID &_binarization_ID,
                       const Cabac_binarization_parameters &_cabac_binarization_parameters);

    Cabac_binarization();

    void setContextParameters(const Cabac_context_parameters &_cabac_context_parameters);

    virtual void write(BitWriter *writer) const;
};

/* ----------------------------------------------------------------------------------------------------------- */

class TransformSubseq_cfg {
public:

    TransformSubseq_cfg();

    TransformSubseq_cfg(const Transform_ID_subsym &_transform_ID_subsym, const Support_values &_support_values,
                        const Cabac_binarization &_cabac_binarization);



    virtual void write(BitWriter *writer) const;

    std::unique_ptr<TransformSubseq_cfg> clone() const {
        auto ret = make_unique<TransformSubseq_cfg>();
        ret->transform_ID_subsym = transform_ID_subsym;
        ret->support_values = support_values;
        ret->cabac_binarization = cabac_binarization;
        return ret;
    }

private:
    Transform_ID_subsym transform_ID_subsym; // : 3
    Support_values support_values;
    Cabac_binarization cabac_binarization;
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
 * Base for ISO 23092-2 Section 8.3.1 table lines 5 to 12
 */
class Descriptor_subsequence_cfg {
private:
    uint16_t descriptor_subsequence_ID : 10; //!< line 5
    std::unique_ptr<Transform_subseq_parameters> transform_subseq_parameters; //!< lines 6 + 7
    std::vector<std::unique_ptr<TransformSubseq_cfg>> transformSubseq_cfgs; //!< lines 8 to 12

public:
    Descriptor_subsequence_cfg();

    Descriptor_subsequence_cfg(std::unique_ptr<Transform_subseq_parameters> _transform_subseq_parameters, uint16_t descriptor_subsequence_ID);

    void addTransformSubseqCfg(std::unique_ptr<TransformSubseq_cfg> _transformSubseq_cfg);

    std::unique_ptr<Descriptor_subsequence_cfg> clone() const {
        auto ret = make_unique<Descriptor_subsequence_cfg>();
        ret->descriptor_subsequence_ID = descriptor_subsequence_ID;
        ret->transform_subseq_parameters = transform_subseq_parameters->clone();
        for(const auto& c : transformSubseq_cfgs) {
            ret->transformSubseq_cfgs.push_back(c->clone());
        }
        return ret;
    }

    virtual void write(BitWriter *writer) const;
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
 * Base for ISO 23092-2 Section 3.3.2.1 table 8 lines 4 to 8
 */
class Decoder_configuration {
public:
    enum class Encoding_mode_ID : uint8_t {
        CABAC = 0 //!< See Text in section
    };

    virtual void write(BitWriter *writer) const {
        writer->write(uint8_t (encoding_mode_ID), 8);
    }

    virtual void addSubsequenceCfg(std::unique_ptr<Descriptor_subsequence_cfg> cfg) = 0;

    virtual std::unique_ptr<Decoder_configuration> clone() const = 0;

    explicit Decoder_configuration(Encoding_mode_ID _encoding_mode_id) : encoding_mode_ID(_encoding_mode_id) {

    }

    virtual ~Decoder_configuration() = default;

protected:
    Encoding_mode_ID encoding_mode_ID; //!< : 8; Line 4
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
 * Base for ISO 23092-2 Section 8.3.1 and ISO 23092-2 Section 8.3.5
 */
class Decoder_configuration_cabac : public Decoder_configuration{
protected:

public:
    Decoder_configuration_cabac () : Decoder_configuration(Encoding_mode_ID::CABAC) {

    }
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
 * ISO 23092-2 Section 8.3.1 table line 3 to 13
 */
class Decoder_configuration_cabac_regular : public Decoder_configuration_cabac {
private:
    uint8_t num_descriptor_subsequence_cfgs_minus1 : 8; //!< Line 3
    std::vector<std::unique_ptr<Descriptor_subsequence_cfg>> descriptor_subsequence_cfgs; //!< Line 4 to 13
public:
    Decoder_configuration_cabac_regular();

    void addSubsequenceCfg(std::unique_ptr<Descriptor_subsequence_cfg> cfg) override;

    std::unique_ptr<Decoder_configuration> clone() const override {
        auto ret = make_unique<Decoder_configuration_cabac_regular>();
        ret->encoding_mode_ID = encoding_mode_ID;
        for (const auto &i : descriptor_subsequence_cfgs) {
            ret->addSubsequenceCfg(i->clone());
        }
        return ret;
    }

    void write(BitWriter *writer) const override;
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
* ISO 23092-2 Section 3.3.2.1 table 8
*/
class Descriptor_configuration {
public:
    enum class Dec_cfg_preset : uint8_t {
        PRESENT = 0 //!< See text in section 3.3.2.1
    };

    explicit Descriptor_configuration(Dec_cfg_preset _dec_cfg_preset) : dec_cfg_preset(_dec_cfg_preset) {

    }

    virtual std::unique_ptr<Descriptor_configuration> clone() const = 0;

    virtual void write(BitWriter *writer) const;

    virtual ~Descriptor_configuration() = default;

protected:
    Dec_cfg_preset dec_cfg_preset : 8; //!< Line 2
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
* ISO 23092-2 Section 3.3.2.1 table 8, lines 4 to 9
*/
class Descriptor_configuration_present : public Descriptor_configuration {
private:
    std::unique_ptr<Decoder_configuration> decoder_configuration; //!< Line 5 to 9 (fused)
public:
    explicit Descriptor_configuration_present() : Descriptor_configuration(Dec_cfg_preset::PRESENT),
                                                  decoder_configuration(nullptr) {
        decoder_configuration = make_unique<Decoder_configuration_cabac_regular>();

    }

    std::unique_ptr<Descriptor_configuration> clone() const override {
        auto ret = make_unique<Descriptor_configuration_present>();
        ret->dec_cfg_preset = dec_cfg_preset;
        ret->decoder_configuration = decoder_configuration->clone();
        return ret;
    }

    void write(BitWriter *writer) const override {
        Descriptor_configuration::write(writer);
        if (dec_cfg_preset != Dec_cfg_preset::PRESENT) {
            return;
        }
        decoder_configuration->write(writer);
    }

    void set_decoder_configuration(std::unique_ptr<Decoder_configuration> conf);

    void _deactivate(); // TODO: get rid of this
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
* ISO 23092-2 Section 3.3.2 table 7 lines 15 to 21
*/
class Descriptor {
public:
    Descriptor();

    void setConfig(uint8_t index, std::unique_ptr<Descriptor_configuration> conf); //!< For class specific config
    void setConfig(std::unique_ptr<Descriptor_configuration> conf); //!< For non-class-specific config
    void enableClassSpecificConfigs(uint8_t numClasses); //!< Unlocks class specific config

    virtual void write(BitWriter *writer) const;

private:
    bool class_specific_dec_cfg_flag : 1; //!< Line 15
    std::vector<std::unique_ptr<Descriptor_configuration>> descriptor_configurations; //!< Lines 16 to 21; Branches fused as data type is the same
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
* ISO 23092-2 Section 3.3.2 table 7 lines 32 to 40
*/
class Qv_coding_config {
public:

    /**
     * Inserted for convenience, not named in ISO 23092-2
     */
    enum class Qv_coding_mode : uint8_t {
        ONE = 1
    };

    virtual void write(BitWriter *writer) const = 0;

    virtual ~Qv_coding_config() = default;

    Qv_coding_config(
            Qv_coding_mode _qv_coding_mode,
            bool _qv_reverse_flag
    );

protected:
    Qv_coding_mode qv_coding_mode; //!< : 4; Line 32
    bool qv_reverse_flag : 1; //!< Line 40
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
* ISO 23092-2 Section 3.3.2 table 7 lines 34 to 38
*/
class Qv_coding_config_1 : public Qv_coding_config {
public:
    /**
    * ISO 23092-2 Section 3.3.2.2.1
    */
    enum class Qvps_preset_ID {
        ASCII = 0, //!< 3.3.2.2.1.1
        OFFSET33_RANGE41 = 1, //!< 3.3.2.2.1.2
        OFFSET64_RANGE40 = 2 //!< 3.3.2.2.1.3
    };

    Qv_coding_config_1();

    explicit Qv_coding_config_1(Qvps_preset_ID _qvps_preset_ID, bool _reverse_flag);

    void setQvps(std::unique_ptr<Parameter_set_qvps> _parameter_set_qvps);

    void write(BitWriter *writer) const override;

private:
    uint8_t qvps_flag : 1; //!< Line 34
    std::unique_ptr<Parameter_set_qvps> parameter_set_qvps; //!< Line 36
    std::unique_ptr<Qvps_preset_ID> qvps_preset_ID; //!< : 4; Line 38
};

/* ----------------------------------------------------------------------------------------------------------- */

/**
* ISO 23092-2 Section 3.3.1 table 6 + 3.3.2 table 7 (fused for simplification)
*/
class Parameter_set : public Data_unit {
public:

    /**
    * ISO 23092-2 Section 5.1 table 34
    */
    enum class AlphabetID : uint8_t {
        ACGTN = 0, //!< Line 1
        ACGTRYSWKMBDHVN_ = 1 //!< Line 2
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

    void setCrps(std::unique_ptr<Parameter_set_crps> parameter_set_crps);

    void addClass(AU_type class_id, std::unique_ptr<Qv_coding_config> conf);

    void setDescriptor(uint8_t index, std::unique_ptr<Descriptor> descriptor);

    void addGroup(std::unique_ptr<std::string> rgroup_id);

    void setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size);

    void write(BitWriter *writer) override;

private:

    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
     *
     * ------------------------------------------------------------------------------------------------------------ */

    uint16_t reserved : 10; //!< Line 8
    uint32_t data_unit_size : 22; //!< Line 9

    /** ----------------------------------------------------------------------------------------------------------- */

    /**
    * ISO 23092-2 Section 3.3.2 table 6
    *
    * ------------------------------------------------------------------------------------------------------------- */

    uint8_t parameter_set_ID : 8; //!< Line 2
    uint8_t parent_parameter_set_ID : 8; //!< Line 3

    /** ----------------------------------------------------------------------------------------------------------- */

    /**
    * ISO 23092-2 Section 3.3.2 table 7
    *
    * ------------------------------------------------------------------------------------------------------------- */

    Dataset_type dataset_type; //!< : 4; Line 2
    AlphabetID alphabet_ID : 8; //!< Line 3
    uint32_t read_length : 24; //!< Line 4
    uint8_t number_of_template_segments_minus1 : 2; //!< Line 5
    uint8_t reserved_2 : 6; //!< Line 6
    uint32_t max_au_data_unit_size : 29; //!< Line 7
    uint8_t pos_40_bits_flag : 1; //!< Line 8
    uint8_t qv_depth : 3; //!< Line 9
    uint8_t as_depth : 3; //!< Line 10
    uint8_t num_classes : 4; //!< Line 11
    std::vector<AU_type> class_IDs; //!< : 4; For loop Lines 12 + 13
    std::vector<std::unique_ptr<Descriptor>> descriptors; //!< For loop lines 14 - 22
    uint16_t num_groups : 16; //!< Line 23
    std::vector<std::unique_ptr<std::string>> rgroup_IDs; //!< For Loop lines 24 + 25
    uint8_t multiple_alignments_flag : 1; //!< Line 26
    uint8_t spliced_reads_flag : 1; //!< Line 27
    uint32_t multiple_signature_base : 31; //!< Line 28
    std::unique_ptr<uint8_t> u_signature_size; //!< : 6; Line 29 + 30
    std::vector<std::unique_ptr<Qv_coding_config>> qv_coding_configs; //!< For loop lines 31 to 41
    uint8_t crps_flag : 1; //!< Line 42
    std::unique_ptr<Parameter_set_crps> parameter_set_crps; //!< Lines 43 + 44

    /** Padding in write() //!< Line 45 + 46 */

    /** ----------------------------------------------------------------------------------------------------------- */

    uint64_t internalBitCounter; // TODO: get rid of this
    void addSize(uint64_t bits);
};

/* ----------------------------------------------------------------------------------------------------------- */

Parameter_set create_quick_parameter_set(uint8_t _parameter_set_id,
                                         uint8_t _read_length,
                                         bool paired_end,
                                         bool qv_values_present,
                                         const std::vector<std::vector<gabac::EncodingConfiguration>> &parameters
);

/* ----------------------------------------------------------------------------------------------------------- */

#endif //GENIE_PART2_FORMAT_PARAMETER_SET_H
