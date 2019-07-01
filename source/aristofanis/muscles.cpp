#include <vector>
#include <string>
#include <algorithm>

const uint8_t Class_I = 1;
const uint8_t Class_HM = 2;
const uint8_t Class_P = 3;
const uint8_t Class_N = 4;
const uint8_t Class_M = 5;
const uint8_t Class_U = 6;

const uint8_t equality_coding = 7;
const uint8_t match_coding = 8;
const uint8_t rle_coding = 9;
const uint8_t merge_coding = 10;

namespace DataStructures
{
	/******* Expandable Array *******/

	template<class T>
	class ExpandableArray 
	{
	private:
		std::vector<T> vec;

	public:
		ExpandableArray();
		T& operator[](size_t);
		const T& operator[](size_t) const;
		size_t size();
		void clear();
	};

	template<class T>
	ExpandableArray::ExpandableArray() 
	{
		vec = std::vector<T>();
	}

	template<class T>
	T& ExpandableArray::operator[](size_t idx) 
	{
		while(vec.size() <= idx)
			vec.push_back(T());
		return vec[idx];
	}

	template<class T>
	const T& ExpandableArray::operator[](size_t idx) const 
	{
		while(vec.size() <= idx)
			vec.push_back(T());
		return vec[idx];
	}

	template<class T>
	size_t ExpandableArray::size() 
	{
		return vec.size();
	}

	template<class T>
	void ExpandableArray::clear() 
	{
		vec.clear();
	}


	/******* Decoder Configuration *******/

	class DecoderConfiguration
	{
	public:
		uint8_t num_descriptor_subsequence_cfgs_minus1;
		DataStructures::ExpandableArray<uint16_t> descriptor_subsequence_ID;

		/******* transform_subseq_parameters() *******/

		DataStructures::ExpandableArray<uint8_t> transformSubseqCounter;
		DataStructures::ExpandableArray<uint8_t> transform_ID_subseq;
		DataStructures::ExpandableArray<uint16_t> match_coding_buffer_size;
		DataStructures::ExpandableArray<uint8_t> rle_coding_guard;
		DataStructures::ExpandableArray<uint8_t> merge_coding_subseq_count;
		DataStructures::ExpandableArray<uint8_t> merge_coding_shift_size;

		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > transform_ID_subsym;

		/******* support_values() *******/

		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > output_symbol_size;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > coding_subsym_size;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > coding_order;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > share_subsym_lut_flag;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > share_subsym_prv_flag;

		/******* cabac_binarization() *******/

		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > binarization_ID;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > bypass_flag;

		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > cmax;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > cmax_teg;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > cmax_dtu;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > split_unit_size;

		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > adaptive_mode_flag;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint16_t> > num_contexts;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > > context_initialization_value;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > share_subsym_ctx_flag;	
	};

	/******* Decoder Configuration TokenType *******/

	class DecoderConfigurationTokenType
	{
	public:
		// adding comments cause they might be relevant
		/* configuration of RLE specified in subclause 10.4.19.3.3 */
		uint8_t rle_guard;
		/* configuration for CABAC_METHOD_0 specified in subclause 10.4.19.3.4 */
		// decoder_configuration_tokentype_cabac(0);
		/* configuration for CABAC_METHOD_1 specified in subclause 10.4.19.3.5 */
		// decoder_configuration_tokentype_cabac(0);

		/******* transform_subseq_parameters() *******/

		// uint8_t transformSubseqCounter; // how to initialize this?
		uint8_t transform_ID_subseq;
		uint16_t match_coding_buffer_size;
		uint8_t rle_coding_guard;
		uint8_t merge_coding_subseq_count;
		DataStructures::ExpandableArray<uint8_t> merge_coding_shift_size;

		DataStructures::ExpandableArray<uint8_t> transform_ID_subsym;

		/******* support_values() *******/

		DataStructures::ExpandableArray<uint8_t> output_symbol_size;
		DataStructures::ExpandableArray<uint8_t> coding_subsym_size;
		DataStructures::ExpandableArray<uint8_t> coding_order;
		DataStructures::ExpandableArray<uint8_t> share_subsym_lut_flag;
		DataStructures::ExpandableArray<uint8_t> share_subsym_prv_flag;

		/******* cabac_binarization() *******/

		DataStructures::ExpandableArray<uint8_t> binarization_ID;
		DataStructures::ExpandableArray<uint8_t> bypass_flag;

		DataStructures::ExpandableArray<uint8_t> cmax;
		DataStructures::ExpandableArray<uint8_t> cmax_teg;
		DataStructures::ExpandableArray<uint8_t> cmax_dtu;
		DataStructures::ExpandableArray<uint8_t> split_unit_size;

		DataStructures::ExpandableArray<uint8_t> adaptive_mode_flag;
		DataStructures::ExpandableArray<uint16_t> num_contexts;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > context_initialization_value;
		DataStructures::ExpandableArray<uint8_t> share_subsym_ctx_flag;

	};

	/******* Descriptor Configuration *******/

	class DescriptorConfiguration
	{
	public:
		uint8_t def_cfg_preset;
		uint8_t encoding_mode_ID;

		DataStructures::DecoderConfiguration dcnfg;
		DataStructures::DecoderConfigurationTokenType dcnfg_tokentype;

		/* Might expand in the future */

	};

	/******* Parameter Set *******/

	class ParameterSet 
	{
	public:
		uint8_t parameter_set_ID;					// unique identifier of the parameter set
		uint8_t parent_parameter_set_ID;			// unique identifier of an existing parameter set.
													// referencing an existing parameter set from another
													// parameter set enables the generation of a hierarchy
													// of parameter sets where the values of the encoding 
													// parameters of each element override the corresponding 
													// values of the parent node (equal to parameter_set_ID
													// when root)

		

		uint8_t dataset_type;
		uint8_t alphabet_ID;
		uint32_t read_length;
		uint8_t number_of_template_segments_minus1;
		/* uint8_t reserved; */
		uint32_t max_au_data_unit_size;
		uint8_t pos_40_bits_flag; // could be bool
		uint8_t qv_depth;
		uint8_t as_depth;
		uint8_t num_classes;

		DataStructures::ExpandableArray<uint8_t> class_ID;
		DataStructures::ExpandableArray<uint8_t> class_specific_dec_cfg_flag;

		/******* Descriptor Configuration (for all classes) *******/

		DataStructures::DescriptorConfiguration descriptor_configuration;

		/******* Descriptor Configuration (one for each class) *******/

		DataStructures::ExpandableArray<DataStructures::DescriptorConfiguration> class_specific_descriptor_configuration;

		uint16_t num_groups;

		DataStructures::ExpandableArray<std::string> rgroup_ID;

		uint16_t multiple_alignments_flag;
		uint16_t spliced_reads_flag;
		uint32_t multiple_signature_base;
		uint8_t U_signature_size;

		DataStructures::ExpandableArray<uint8_t> qv_coding_mode;
		DataStructures::ExpandableArray<uint8_t> qvps_flag;
		DataStructures::ExpandableArray<uint8_t> qvps_preset_ID;

		/******* Parameter Set QVPS *******/

		uint8_t qv_num_codebooks_total;
		DataStructures::ExpandableArray<uint8_t> qv_num_codebook_entries;
		DataStructures::ExpandableArray<DataStructures::ExpandableArray<uint8_t> > qv_recon;

		uint8_t crps_flag;

		/******* Parameter Set CRPS *******/

		uint8_t cr_alg_ID;
		uint8_t cr_pad_size;
		uint32_t cr_buf_max_size;

		uint8_t nesting_zero_bit; 
	};

	
	/******* Block *******/

	class Block
	{
	public:
		/* uint8_t reserved; */ // dont need this
		uint8_t descriptor_ID;
		/* uint8_t reserved_prime; */ // ??? why twice ask 7.4.1.2.1
		uint32_t block_payload_size;

		/******* Encoded TokenType *******/ // 10.4.19.1

		uint32_t num_output_descriptors;
		uint16_t num_tokentype_sequences;

		DataStructures::ExpandableArray<uint8_t> type_ID;
		DataStructures::ExpandableArray<uint8_t> method_ID;
		DataStructures::ExpandableArray<uint16_t> ref_type_ID;
		// COP(i) ??? what is this

		DataStructures::ExpandableArray<uint64_t> num_output_symbols; // note: u7(v)
		// decode_tokentype_sequence(i, method_ID, num_output_symbols) ???
		
		/******* Encoded Descriptor Sequences *******/ // 12.6.2.1

		uint8_t nesting_zero_bit;

	};

	/******* Access Unit *******/

	class AccessUnit 
	{
	public:
		/******* Access Unit Header *******/

		uint32_t access_unit_ID;
		uint8_t num_blocks;
		uint8_t parameter_set_ID;
		uint8_t AU_type; // should be 4-bits
		uint32_t read_count;

		uint16_t mm_threshold;
		uint16_t mm_count;

		uint16_t ref_sequence_ID;
		uint64_t ref_start_position;
		uint64_t ref_end_position;

		uint16_t sequence_ID;
		uint64_t AU_start_position;
		uint64_t AU_end_position;

		uint64_t extended_AU_start_position;
		uint64_t extended_AU_end_position

		DataStructures::ExpandableArray<uint64_t> U_cluster_signature;
		uint16_t num_signatures;
		uint8_t nesting_zero_bit;

		/******* Blocks *******/

		DataStructures::ExpandableArray<Block> blocks;


	};
}


namespace HelperFunctions
{
	void zeroOrderComplement(uint8_t idx) 
	{
		if(idx == 4) return 4;
		return 3-idx;
	}

	void firstOrderComplement(uint8_t idx)
	{
		if(idx <= 3) return 3-idx;
		if(idx <= 5) return 5-idx  + 4; 
		if(idx <= 7) return idx;
		if(idx <= 9) return 9-idx + 8;
		if(idx <= 13) return 13-idx + 10;
		return idx;
	}

	void initializeSupportedSymbolsVector(DataStructures::ExpandableArray<std::string> &supportedSymbols) 
	{
		supportedSymbols.clear();
		supportedSymbols[0] = "ACGTN";
		supportedSymbols[1] = "ACGTRYSWKMBDHVN-";
	}

	// LSB -> MSB
	uint64_t parseU7Forward(uint64_t value)
	{
		// TODO
	}

	// MSB -> LSB
	uint64_t parseU7Reverse(uint64_t value)
	{
		// TODO
	}

}

namespace Decoder
{
	void decode() 
	{

	}
}