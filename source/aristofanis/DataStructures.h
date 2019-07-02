#include <vector>
#include <string>
#include <algorithm>

#define CLASS_P     1
#define CLASS_N     2
#define CLASS_M     3
#define CLASS_I     4
#define CLASS_HM    5
#define CLASS_U     6

#define EQUALITY_CODING     7
#define MATCH_CODING        8
#define RLE_CODING          9
#define MERGE_CODING        10

namespace helper_functions {

    void zero_order_complement(uint8_t);
    void first_order_complement(uint8_t);
    void initialize_supported_symbols(data_structures::expandable_array<std::string> &);

}

namespace data_structures {

    template<typename T>
    struct expandable_array {
    private:
        std::vector<T> vec;

    public:
        typedef std::vector<T>::iterator                iterator;
        typedef std::vector<T>::const_iterator          const_iterator;
        typedef std::vector<T>::reverse_iterator        reverse_iterator;
        typedef std::vector<T>::const_reverse_iterator  const_reverse_iterator;

        expandable_array();
        expandable_array(size_t);
        expandable_array(size_t, const T&);
        expandable_array(const_iterator, const_iterator);

        T&          operator[](size_t);
        const T&    operator[](size_t) const;
        size_t      size();
        void        clear();
        bool        empty();

        iterator        begin();
        iterator        end();
        const_iterator  begin() const;
        const_iterator  end() const;

        reverse_iterator        rbegin();
        reverse_iterator        rend();
        reverse_const_iterator  rbegin() const;
        reverse_const_iterator  rend() const;
    };

    struct COP              { COP(); COP(uint16_t); }; // this one should raise an exception
    struct RLE              { RLE(); RLE(uint16_t, uint64_t); }; // this one should raise an exception
    struct CABAC_METHOD_0   { CABAC_METHOD_0(); CABAC_METHOD_0(uint16_t, uint64_t); }; // this one should raise an exception
    struct CABAC_METHOD_1   { CABAC_METHOD_1(); CABAC_METHOD_1(uint16_t, uint64_t); }; // this one should raise an exception
    struct X4               { X4(); X4(uint16_t, uint64_t); }; // this one should raise an exception
    
    struct CAT { 
        expandable_array<expandable_array<uint8_t> > decoded_tokens;
        CAT(); 
        CAT(uint16_t, uint64_t); 
    };

    struct decode_tokentype_sequence {
        CAT             cat;
        RLE             rle;
        CABAC_METHOD_0  cm0;
        CABAC_METHOD_1  cm1;
        X4              x4;

        decode_tokentype_sequence();
        decode_tokentype_sequence(uint16_t, uint8_t, uint64_t); // might throw exception
    }

    struct encoded_tokentype_sequence {
        uint8_t         type_ID;
        uint8_t         method_ID;

        uint16_t        ref_type_ID;    // reserved for future use, since we are currenty assuming method_ID = 1
        COP             cop;            // same here

        uint64_t                    num_output_symbols;
        decode_tokentype_sequence   dec_tt_seq;

        encoded_tokentype_sequence();
        encoded_tokentype_sequence(uint16_t, uint8_t);  // reserved for future use
                                                        // creates COP(i) object when method_ID = 0
        encoded_tokentype_sequence(uint16_t, uint8_t, uint64_t);    // creates decode_tokentype_sequence object
                                                                    // i, method_ID, num_output_symbols
    };

    // 10.4.19.1
    struct encoded_tokentype {
        uint32_t        num_output_descriptors;
        uint16_t        num_tokentype_sequences;

        expandable_array<encoded_tokentype_sequence>    enc_tt_seq;
    };

    // 12.6.2.1
    struct encoded_descriptor_sequences {
        // Probably don't need to implement this for my purposes
        // TODO ask to be sure
    };

    struct block_header {
        uint8_t         descriptor_ID:
        uint32_t        block_payload_size;
    };

    struct block_payload {
        encoded_tokentype               enc_tt;
        encoded_descriptor_sequences    enc_desc_seq;
        bool                            nesting_zero_bit;

        block_payload();
        block_payload(uint8_t);
    };

    struct access_unit_header {
        uint32_t        access_unit_ID;
        uint8_t         num_blocks;
        uint8_t         parameter_set_ID;
        uint8_t         AU_type;
        uint32_t        read_count;
        
        uint16_t        mm_threshold;
        uint32_t        mm_count;
        
        uint16_t        ref_sequence_ID;
        uint64_t        ref_start_position;
        uint64_t        ref_end_position;
        
        uint16_t        sequence_ID;
        uint64_t        AU_start_position;
        uint64_t        AU_end_position;
        uint64_t        extended_AU_start_position;
        uint64_t        extended_AU_end_position;

        expandable_array<uint64_t>  U_cluster_signature;
        uint16_t                    num_signatures;
        bool                        nesting_zero_bit;
    };

    struct access_unit_block {
        block_header        header;
        block_payload       payload;
    };

    struct access_unit {
        access_unit_header                      header;
        expandable_array<access_unit_block>     blocks;

        // kinda different from the specification, however it serves my purpose better
        expandable_array<expandable_array<expandable_array<uint64_t> > >        decoded_symbols;
        expandable_array<expandable_array<uint64_t> > >                         j;
    };

    struct transform_subseq_parameters {
        uint8_t                     transform_ID_subseq;
        uint16_t                    match_coding_buffer_size;
        uint8_t                     rle_coding_guard;
        uint8_t                     merge_coding_subseq_count;
        expandable_array<uint8_t>   merge_coding_shift_size;
    };

    struct support_values {
        uint8_t         output_symbol_size;
        uint8_t         coding_subsym_size;
        uint8_t         coding_order;

        bool        share_subsym_lut_flag;
        bool        share_subsym_prv_flag;
    };

    struct cabac_binarization_parameters {
        uint8_t         cmax;
        uint8_t         cmax_teg;
        uint8_t         cmax_dtu;
        uint8_t         split_unit_size;
    };

    struct cabac_context_parameters {
        bool                        adaptive_mode_flag;
        uint16_t                    num_contexts;
        expandable_array<uint8_t>   context_initialization_value;
        bool                        share_subsym_ctx_flag;
    };

    struct cabac_binarization {
        uint8_t         binarization_ID;
        bool            bypass_flag;

        cabac_binarization_parameters       cabac_bin_params;
        cabac_context_parameters            cabac_context_params;
    };

    struct decoder_configuration {
        uint8_t                                         num_descriptor_subsequence_cfgs_minus1;
        expandable_array<uint16_t>                      descriptor_subsequence_ID;
        expandable_array<transform_subseq_parameters>   transform_subseq_params;

        expandable_array<expandable_array<uint8_t> >                transform_ID_subsym;
        expandable_array<expandable_array<support_values> >         supp_vals;
        expandable_array<expandable_array<cabac_binarization> >     cabac_bin;
    };

    struct decoder_configuration_tokentype_cabac {
        transform_subseq_parameters             transform_subseq_params;
        expandable_array<uint8_t>               transform_ID_subsym;
        expandable_array<support_values>        supp_vals;
        expandable_array<cabac_binarization>    cabac_bin;
    };

    struct decoder_configuration_tokentype {
        uint8_t                                 rle_guard_tokentype;
        decoder_configuration_tokentype_cabac   cfg_cm0;
        decoder_configuration_tokentype_cabac   cfg_cm1;
    };

    struct descriptor_configuration {
        uint8_t         dec_cfg_preset;
        uint8_t         encoding_mode_ID;

        decoder_configuration               dec_cfg;
        decoder_configuration_tokentype     dec_cfg_tt;

        /* reserved for future use */
    };

    struct parameter_set_qvps {
        uint8_t                                         qv_num_codebooks_total;
        expandable_array<uint8_t>                       qv_num_codebook_entries;
        expandable_array<expandable_array<uint8_t> >    qv_recon;
    };

    struct parameter_set_crps {
        uint8_t         cr_alg_ID;
        uint8_t         cr_pad_size;
        uint8_t         cr_buf_max_size;
    };

    struct encoding_parameters {
        uint8_t         dataset_type;
        uint8_t         alphabet_ID;
        uint32_t        read_length;
        uint8_t         number_of_template_segments_minus1
        uint8_t         max_au_data_unit_size;
        uint32_t        pos_40_bits_flag;
        bool            qv_depth;
        uint8_t         as_depth;
        uint8_t         num_classes;

        expandable_array<uint8_t>       class_ID;
        expandable_array<bool>          class_specific_dec_cfg_flag;

        expandable_array<descriptor_configuration>                      desc_cfg;
        expandable_array<expandable_array<descriptor_configuration> >   class_specific_desc_cfg; // 7.3.2.1

        uint16_t                        num_groups;
        expandable_array<std::string>   rgroup_ID;
        bool                            multiple_alignments_flag;
        bool                            spliced_reads_flag;
        uint32_t                        multiple_signature_base;
        uint8_t                         U_signature_size;

        expandable_array<uint8_t>               qv_coding_mode;
        expandable_array<bool>                  qvps_flag;
        expandable_array<parameter_set_qvps>    param_set_qvps; // 7.3.2.2
        expandable_array<uint8_t>               qvps_preset_ID;

        bool                                    crps_flag;
        parameter_set_crps                      param_set_crps; // 7.3.2.3

        bool                            nesting_zero_bit;
    };

    struct parameter_set {
        uint8_t parameter_set_ID;
        uint8_t parent_parameter_set_ID;

        encoding_parameters enc_params;
    };

}