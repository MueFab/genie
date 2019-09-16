#include <map>
#include <iostream>
#include "read.h"
#include "gabac-subset/bit_input_stream.h"
#include "gabac-subset/data_block.h"
#include "gabac-subset/block_stepper.h"

#ifndef PARAMETERSET_H
#define PARAMETERSET_H

struct TransformSubseqParameters {
	uint64_t transform_ID_subseq;
	uint64_t match_coding_buffer_size;
	uint64_t rle_coding_guard;
	uint64_t merge_coding_subseq_count;
	uint64_t *merge_coding_shift_size;

	TransformSubseqParameters() {}
	TransformSubseqParameters(gabac::BitInputStream &instream, uint64_t &transformSubseqCounter) {
		transform_ID_subseq = readUnsigned(instream, 8);
		if(transform_ID_subseq == 1) /* equality coding */
			transformSubseqCounter += 1;
		else if(transform_ID_subseq == 2) /* match coding */ {
			match_coding_buffer_size = readUnsigned(instream, 16);
			transformSubseqCounter += 2;
		}
		else if(transform_ID_subseq == 3) /* rle coding */ {
			rle_coding_guard = readUnsigned(instream, 8);
			transformSubseqCounter += 1;
		}
		else if(transform_ID_subseq == 4) /* merge coding */ {
			merge_coding_subseq_count = readUnsigned(instream, 4);
			transformSubseqCounter = merge_coding_subseq_count;
			merge_coding_shift_size = new uint64_t[merge_coding_subseq_count];
			for(size_t i=0; i<merge_coding_subseq_count; ++i)
				merge_coding_shift_size[i] = readUnsigned(instream, 5);
		}

		std::cout << DEBUG(transformSubseqCounter) << std::endl;
	}
};

struct SupportValues {
	uint64_t output_symbol_size;
	uint64_t coding_subsym_size;
	uint64_t coding_order;
	uint64_t share_subsym_lut_flag;
	uint64_t share_subsym_prv_flag;

	SupportValues() {}
	SupportValues(gabac::BitInputStream &instream, uint64_t transform_ID_subsym) {
		output_symbol_size = readUnsigned(instream, 6);
		coding_subsym_size = readUnsigned(instream, 6);
		coding_order = readUnsigned(instream, 2);
		if(coding_subsym_size < output_symbol_size && coding_order > 0) {
			if(transform_ID_subsym == 1)
				share_subsym_lut_flag = readUnsigned(instream, 1);
			share_subsym_prv_flag = readUnsigned(instream, 1);
		}
	}
};

struct CabacBinarizationParameters {
	uint64_t cmax;
	uint64_t cmax_teg;
	uint64_t cmax_dtu;
	uint64_t split_unit_size;

	CabacBinarizationParameters() {}
	CabacBinarizationParameters(gabac::BitInputStream &instream, uint64_t binarization_ID) {
		if(binarization_ID == 1)
			cmax = readUnsigned(instream, 8);
		else if(binarization_ID == 4 || binarization_ID == 5)
			cmax_teg = readUnsigned(instream, 8);
		else if(binarization_ID == 8 || binarization_ID == 9)
			cmax_dtu = readUnsigned(instream, 8);

		if (binarization_ID == 6 || binarization_ID == 7 || 
			binarization_ID == 8 || binarization_ID == 9)
			split_unit_size = readUnsigned(instream, 4);
	}
};

struct CabacContextParameters {
	uint64_t adaptive_mode_flag;
	uint64_t num_contexts;
	uint64_t *context_initialization_value;
	uint64_t share_subsym_ctx_flag;

	CabacContextParameters() {}
	CabacContextParameters(gabac::BitInputStream &instream, bool boolean) {
		adaptive_mode_flag = readUnsigned(instream, 1);
		num_contexts = readUnsigned(instream, 16);
		context_initialization_value = new uint64_t[num_contexts];

		for(size_t i=0; i<num_contexts; ++i)
			context_initialization_value[i] = readUnsigned(instream, 7);

		if(boolean)
			share_subsym_ctx_flag = readUnsigned(instream, 1);
	}
};

struct CabacBinarization {
	uint64_t binarization_ID;
	uint64_t bypass_flag;

	CabacBinarizationParameters cabac_binarization_parameters;
	CabacContextParameters cabac_context_parameters;

	CabacBinarization() {}
	CabacBinarization(gabac::BitInputStream &instream, bool boolean) {
		binarization_ID = readUnsigned(instream, 5);
		bypass_flag = readUnsigned(instream, 1);
		cabac_binarization_parameters = CabacBinarizationParameters(instream, binarization_ID);
		if(!bypass_flag)
			cabac_context_parameters = CabacContextParameters(instream, boolean);
	}
};

struct DecoderConfiguration {
	uint64_t num_descriptor_subsequence_cfgs_minus1;
	uint64_t *descriptor_subsequence_ID;
	uint64_t **transform_ID_subsym;

	TransformSubseqParameters *transform_subseq_parameters;
	SupportValues **support_values;	
	CabacBinarization **cabac_binarization;

	DecoderConfiguration() {}
	DecoderConfiguration(gabac::BitInputStream &instream, uint64_t encoding_mode_ID) {
		if(encoding_mode_ID == 0) {
			num_descriptor_subsequence_cfgs_minus1 = readUnsigned(instream, 8);
			descriptor_subsequence_ID = new uint64_t[num_descriptor_subsequence_cfgs_minus1+1];
			transform_subseq_parameters = new TransformSubseqParameters[num_descriptor_subsequence_cfgs_minus1+1];
			transform_ID_subsym = new uint64_t*[num_descriptor_subsequence_cfgs_minus1+1];
			support_values = new SupportValues*[num_descriptor_subsequence_cfgs_minus1+1];
			cabac_binarization = new CabacBinarization*[num_descriptor_subsequence_cfgs_minus1+1];

			for(size_t i=0; i<=num_descriptor_subsequence_cfgs_minus1; ++i) {
				descriptor_subsequence_ID[i] = readUnsigned(instream, 10);
				uint64_t transformSubseqCounter = 1;
				transform_subseq_parameters[i] = TransformSubseqParameters(instream, transformSubseqCounter); // todo, possibly have transformSubseqCounter as reference
				transform_ID_subsym[i] = new uint64_t[transformSubseqCounter];
				support_values[i] = new SupportValues[transformSubseqCounter];
				cabac_binarization[i] = new CabacBinarization[transformSubseqCounter];

				for(size_t j=0; j<transformSubseqCounter; ++j) {
					transform_ID_subsym[i][j] = readUnsigned(instream, 3);
					support_values[i][j] = SupportValues(instream, transform_ID_subsym[i][j]);
					cabac_binarization[i][j] = CabacBinarization(instream, support_values[i][j].coding_subsym_size < support_values[i][j].output_symbol_size);
				}

			}
		} else if(encoding_mode_ID >= 1) {
			/* reserved */
		}
	}
};

struct DecoderConfigurationTokentypeCabac {
	uint64_t *transform_ID_subsym;

	TransformSubseqParameters transform_subseq_parameters;
	SupportValues *support_values;
	CabacBinarization *cabac_binarization;

	DecoderConfigurationTokentypeCabac() {}
	DecoderConfigurationTokentypeCabac(gabac::BitInputStream &instream) {
		uint64_t transformSubseqCounter = 1;
		transform_subseq_parameters = TransformSubseqParameters(instream, transformSubseqCounter);
		support_values = new SupportValues[transformSubseqCounter];
		cabac_binarization = new CabacBinarization[transformSubseqCounter];
		transform_ID_subsym = new uint64_t[transformSubseqCounter];

		for(size_t j=0; j<transformSubseqCounter; ++j) {
			transform_ID_subsym[j] = readUnsigned(instream, 3);
			support_values[j] = SupportValues(instream, transform_ID_subsym[j]);
			cabac_binarization[j] = CabacBinarization(instream, support_values[j].coding_subsym_size < support_values[j].output_symbol_size);
		}
	}
};

struct DecoderConfigurationTokentype {
	uint64_t rle_guard_tokentype;

	DecoderConfigurationTokentypeCabac *decoder_configuration_tokentype_cabac;

	DecoderConfigurationTokentype() {}
	DecoderConfigurationTokentype(gabac::BitInputStream &instream, uint64_t encoding_mode_ID) {
		if(encoding_mode_ID == 0) {
			rle_guard_tokentype = readUnsigned(instream, 8);
			decoder_configuration_tokentype_cabac = new DecoderConfigurationTokentypeCabac[2];
			decoder_configuration_tokentype_cabac[0] = DecoderConfigurationTokentypeCabac(instream);
			decoder_configuration_tokentype_cabac[1] = DecoderConfigurationTokentypeCabac(instream);
		} else if(encoding_mode_ID >= 1) {
			/* reserved */
		}
	}
};

struct DescriptorConfiguration {
	uint64_t dec_cfg_preset;
	uint64_t encoding_mode_ID;

	DecoderConfiguration decoder_configuration;
	DecoderConfigurationTokentype decoder_configuration_tokentype;

	DescriptorConfiguration() {}
	DescriptorConfiguration(gabac::BitInputStream &instream, uint64_t desc_ID) {
		dec_cfg_preset = readUnsigned(instream, 8);
		if(dec_cfg_preset == 0) {
			encoding_mode_ID = readUnsigned(instream, 8);
			if(desc_ID != 11 && desc_ID != 15)
				decoder_configuration = DecoderConfiguration(instream, encoding_mode_ID);
			else if(desc_ID == 11 || desc_ID == 15)
				decoder_configuration_tokentype = DecoderConfigurationTokentype(instream, encoding_mode_ID);
		}
	}
};

static uint64_t NUM_DESCRIPTOR_SUBSEQUENCES[7][NUM_DESCRIPTORS] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 0, 2, 1, 1 },
	{ 2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 0, 2, 1, 1 },
	{ 2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 0, 2, 1, 1 },
	{ 2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 0, 2, 1, 1 },
	{ 2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 0, 2, 1, 1 },
	{ 2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 0, 2, 1, 1 },
};

struct ParameterSetQvps {
	

	uint64_t qv_num_codebooks_total;
	uint64_t *qv_num_codebook_entries;
	uint64_t **qv_recon;

	ParameterSetQvps() {}
	ParameterSetQvps(gabac::BitInputStream &instream, uint64_t class_ID) {
		qv_num_codebooks_total = readUnsigned(instream, 4);

		uint64_t qvNumCodebooksAligned;
		if(class_ID == CLASS_I || class_ID == CLASS_HM)
			qvNumCodebooksAligned = qv_num_codebooks_total - 1;
		else if(class_ID != CLASS_U)
			qvNumCodebooksAligned = qv_num_codebooks_total;
		else
			qvNumCodebooksAligned = 0;

		if(qvNumCodebooksAligned > 1)
			NUM_DESCRIPTOR_SUBSEQUENCES[class_ID][14] = 2;
		else 
			NUM_DESCRIPTOR_SUBSEQUENCES[class_ID][14] = 1;
		

		qv_num_codebook_entries = new uint64_t[qv_num_codebooks_total];
		qv_recon = new uint64_t*[qv_num_codebooks_total];

		for(size_t b=0; b<qv_num_codebooks_total; ++b) {
			qv_num_codebook_entries[b] = readUnsigned(instream, 8);
			qv_recon[b] = new uint64_t[qv_num_codebook_entries[b]];

			for(size_t e=0; e<qv_num_codebook_entries[b]; ++e)
				qv_recon[b][e] = readUnsigned(instream, 8);
		}
	}
};

struct ParameterSetCrps {
	uint64_t cr_alg_ID;
	uint64_t cr_pad_size;
	uint64_t cr_buf_max_size;

	ParameterSetCrps() {}
	ParameterSetCrps(gabac::BitInputStream &instream) {
		cr_alg_ID = readUnsigned(instream, 8);
		if(cr_alg_ID == 2 || cr_alg_ID == 3) {
			cr_pad_size = readUnsigned(instream, 8);
			cr_buf_max_size = readUnsigned(instream, 24);
		}
	}
}; 

struct EncodingParameters {
	uint64_t posSize;
	uint64_t dataset_type;
	uint64_t alphabet_ID;
	uint64_t read_length;
	uint64_t number_of_template_segments_minus1;
	uint64_t max_au_data_unit_size;
	uint64_t pos_40_bits_flag;
	uint64_t qv_depth;
	uint64_t as_depth;
	uint64_t num_classes;
	uint64_t *class_ID;
	uint64_t *class_specific_dec_cfg_flag;
	uint64_t num_groups;
	std::string *rgroup_ID;
	uint64_t multiple_alignments_flag;
	uint64_t spliced_reads_flag;
	uint64_t multiple_signature_base;
	uint64_t U_signature_size;
	uint64_t *qv_coding_mode;
	uint64_t *qvps_flag;
	uint64_t *qvps_preset_ID;
	uint64_t *qv_reverse_flag;
	uint64_t crps_flag;

	DescriptorConfiguration *descriptor_configuration, **descriptor_configuration_class_specific;
	ParameterSetQvps *parameter_set_qvps; // with class_ID[c] as constructor
	ParameterSetCrps parameter_set_crps;
	// nesting_zero_bit // use getNumBitsUntilByteAligned

	EncodingParameters() {}
	EncodingParameters(gabac::BitInputStream &instream) {
		dataset_type = readUnsigned(instream, 4);
		alphabet_ID = readUnsigned(instream, 8);
		read_length = readUnsigned(instream, 24);
		number_of_template_segments_minus1 = readUnsigned(instream, 2);
		readUnsigned(instream, 6); // reserved
		max_au_data_unit_size = readUnsigned(instream, 29);
		pos_40_bits_flag = readUnsigned(instream, 1);

		if(pos_40_bits_flag == 0)
			posSize = 32;
		else
			posSize = 40;

		qv_depth = readUnsigned(instream, 3);
		as_depth = readUnsigned(instream, 3);
		num_classes = readUnsigned(instream, 4);
		class_ID = new uint64_t[num_classes];

		for(size_t j=0; j<num_classes; ++j)
			class_ID[j] = readUnsigned(instream, 4);

		std::cout << DEBUG(num_classes) << std::endl;
		for(size_t j=0; j<num_classes; ++j)
			std::cout << DEBUG(class_ID[j]) << std::endl;

		class_specific_dec_cfg_flag = new uint64_t[NUM_DESCRIPTORS];
		descriptor_configuration = new DescriptorConfiguration[NUM_DESCRIPTORS];
		descriptor_configuration_class_specific = new DescriptorConfiguration*[num_classes];
		
		for(size_t j=0; j<num_classes; ++j)
			descriptor_configuration_class_specific[j] = new DescriptorConfiguration[NUM_DESCRIPTORS];
		
		for(size_t i=0; i<NUM_DESCRIPTORS; ++i) {
			class_specific_dec_cfg_flag[i] = readUnsigned(instream, 1);
			if(class_specific_dec_cfg_flag[i] == 0)
				descriptor_configuration[i] = DescriptorConfiguration(instream, i);
			else {
				for(size_t j=0; j<num_classes; ++j)
					descriptor_configuration_class_specific[j][i] = DescriptorConfiguration(instream, i);
			}
		}

		num_groups = readUnsigned(instream, 16);
		rgroup_ID = new std::string[num_groups];

		for(size_t j=0; j<num_groups; ++j)
			rgroup_ID[j] = readString(instream);
		
		multiple_alignments_flag = readUnsigned(instream, 1);
		spliced_reads_flag = readUnsigned(instream, 1);
		multiple_signature_base = readUnsigned(instream, 31);
		
		if(multiple_signature_base > 0)
			U_signature_size = readUnsigned(instream, 6);

		qv_coding_mode = new uint64_t[num_classes];
		qvps_flag = new uint64_t[num_classes];
		parameter_set_qvps = new ParameterSetQvps[num_classes];
		qvps_preset_ID = new uint64_t[num_classes];
		qv_reverse_flag = new uint64_t[num_classes];

		for(size_t c=0; c<num_classes; ++c) {
			qv_coding_mode[c] = readUnsigned(instream, 4);
			if(qv_coding_mode[c] == 1) {
				qvps_flag[c] = readUnsigned(instream, 1);
				if(qvps_flag[c])
					parameter_set_qvps[c] = ParameterSetQvps(instream, class_ID[c]);
				else
					qvps_preset_ID[c] = readUnsigned(instream, 4);
			}
			qv_reverse_flag[c] = readUnsigned(instream, 1);
		}
		
		crps_flag = readUnsigned(instream, 1);
		if(crps_flag)
			parameter_set_crps = ParameterSetCrps(instream);
		
		readUnsigned(instream, instream.getNumBitsUntilByteAligned()); // nesting_zero_bit
	}
};

struct ParameterSet {
	uint64_t parameter_set_ID;
	uint64_t parent_parameter_set_ID;

	EncodingParameters encoding_parameters;

	ParameterSet() {}
	ParameterSet(gabac::BitInputStream &instream) {
		parameter_set_ID = readUnsigned(instream, 8);
		parent_parameter_set_ID = readUnsigned(instream, 8);

		std::cout << DEBUG(parameter_set_ID) << std::endl;
		std::cout << DEBUG(parent_parameter_set_ID) << std::endl;

		encoding_parameters = EncodingParameters(instream);
	}
};

#endif
