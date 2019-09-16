#include <map>
#include <iostream>
#include <vector>
#include "read.h"
#include "expandable-array.h"
#include "gabac-subset/bit_input_stream.h"
#include "gabac-subset/data_block.h"
#include "gabac-subset/block_stepper.h"

#ifndef ACCESSUNIT_H
#define ACCESSUNIT_H

FILE *fin; // to read the decoded symbols

struct AccessUnit {
	expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols;
	expandable_array<expandable_array<uint64_t> > j;

	struct AccessUnitHeader {
		ParameterSet ps;

		uint64_t access_unit_ID;
		uint64_t num_blocks;
		uint64_t parameter_set_ID;
		uint64_t AU_type;
		uint64_t reads_count;
		uint64_t mm_threshold;
		uint64_t mm_count;
		uint64_t ref_sequence_ID;
		uint64_t ref_start_position;
		uint64_t ref_end_position;
		uint64_t sequence_ID;
		uint64_t AU_start_position;
		uint64_t AU_end_position;
		uint64_t extended_AU_start_position;
		uint64_t extended_AU_end_position;
		uint64_t *U_cluster_signature;
		uint64_t num_signatures;
		// nesting_zero_bit

		ParameterSet getParameterSet(std::vector<ParameterSet> collection) {
			for(size_t i=0; i<collection.size(); ++i) {
				if(collection[i].parameter_set_ID == parameter_set_ID)
					return collection[i];
			}

			assert(!"No parameter set");
		}

		AccessUnitHeader() {}
		AccessUnitHeader(gabac::BitInputStream &instream, std::vector<ParameterSet> collection) {
			access_unit_ID = readUnsigned(instream, 32);
			num_blocks = readUnsigned(instream, 8);
			parameter_set_ID = readUnsigned(instream, 8);
			AU_type = readUnsigned(instream, 4);
			reads_count = readUnsigned(instream, 32);

			if(AU_type == N_TYPE_AU || AU_type == M_TYPE_AU) {
				mm_threshold = readUnsigned(instream, 16);
				mm_count = readUnsigned(instream, 32);
			}

			ps = getParameterSet(collection);

			if(ps.encoding_parameters.dataset_type == 2) {
				ref_sequence_ID = readUnsigned(instream, 16);
				ref_start_position = readUnsigned(instream, ps.encoding_parameters.posSize);
				ref_end_position = readUnsigned(instream, ps.encoding_parameters.posSize);
			}

			if(AU_type != U_TYPE_AU) {
				sequence_ID = readUnsigned(instream, 16);
				AU_start_position = readUnsigned(instream, ps.encoding_parameters.posSize);
				AU_end_position = readUnsigned(instream, ps.encoding_parameters.posSize);

				if(ps.encoding_parameters.multiple_alignments_flag) {
					extended_AU_start_position = readUnsigned(instream, ps.encoding_parameters.posSize);
					extended_AU_end_position = readUnsigned(instream, ps.encoding_parameters.posSize);
				}
			}
			else {
				if(ps.encoding_parameters.multiple_signature_base != 0) {
					U_cluster_signature = new uint64_t[ps.encoding_parameters.multiple_signature_base];
					U_cluster_signature[0] = readUnsigned(instream, ps.encoding_parameters.U_signature_size);

					if(U_cluster_signature[0] != (1<<ps.encoding_parameters.U_signature_size)-1) {
						for(size_t i=1; i<ps.encoding_parameters.multiple_signature_base; ++i)
							U_cluster_signature[i] = readUnsigned(instream, ps.encoding_parameters.U_signature_size);
					}
				}
				else {
					num_signatures = readUnsigned(instream, 16);

					for(size_t i=0; i<num_signatures; ++i)
						U_cluster_signature[i] = readUnsigned(instream, ps.encoding_parameters.U_signature_size);
				}

			}

			readUnsigned(instream, instream.getNumBitsUntilByteAligned());
		}
	} access_unit_header;

	struct Block {
		struct BlockHeader {
			uint64_t descriptor_ID;
			uint64_t block_payload_size;

			BlockHeader() {}
			BlockHeader(gabac::BitInputStream &instream) {
				readUnsigned(instream, 1);
				descriptor_ID = readUnsigned(instream, 7);
				readUnsigned(instream, 3);
				block_payload_size = readUnsigned(instream, 29);
			}
		} block_header;

		struct BlockPayload {
			struct EncodedTokentype {
				/*
				Not necessary just yet
				*/
			} encoded_tokentype;

			/*
			struct EncodedDescriptorSequences {
				uint64_t *subsequence_payload_size;
				uint64_t *num_encoded_symbols;

				EncodedDescriptorSequences() {}
				EncodedDescriptorSequences(gabac::BitInputStream &instream, uint64_t AU_type, uint64_t descriptor_ID, uint64_t block_payload_size, uint64_t &bits_read_count, expandable_array<expandable_array<expandable_array<uint64_t> > > &decoded_symbols) {
					std::cout << "| AU_type, descriptor_ID: " << AU_type << ", " << descriptor_ID << std::endl;

					uint64_t remainingPayloadSize = block_payload_size;
					uint64_t numDescriptorSubsequences = NUM_DESCRIPTOR_SUBSEQUENCES[AU_type][descriptor_ID];
					uint64_t subsequencePayloadSize;

					subsequence_payload_size = new uint64_t[numDescriptorSubsequences];
					num_encoded_symbols = new uint64_t[numDescriptorSubsequences];

					uint64_t line_counter = 0;

					for(size_t k=0; k<numDescriptorSubsequences; ++k) {
						if(k < numDescriptorSubsequences-1) {
							subsequence_payload_size[k] = readUnsigned(instream, 32);
							bits_read_count += 32;
							subsequencePayloadSize = subsequence_payload_size[k];
							remainingPayloadSize -= (subsequencePayloadSize + 4);
						} 
						else
							subsequencePayloadSize = remainingPayloadSize;
						
						if(subsequencePayloadSize > 0) {
							num_encoded_symbols[k] = readUnsigned(instream, 32);
							bits_read_count += 32;
							// decode_descriptor_subsequence 
							// not implementing it; we are taking decoded_symbols from a file
							line_counter++;
						}
					}

					while(line_counter--) {
						
						unsigned long long descID, subseqID, num_symbols;
						fscanf(fin, "%llu %llu %llu", &descID, &subseqID, &num_symbols);

						std::cout << "| descID, subseqID, num_symbols: " << descID << ", " << subseqID << ", " << num_symbols << " > ";

						for(size_t i=0; i<num_symbols; ++i) {
							uint64_t val;
							fscanf(fin, "%llu", &val);
							decoded_symbols[descID][subseqID][i] = val;
							std::cout << val << " ";
						}
						std::cout << std::endl;
					}
				}
			} encoded_descriptor_sequences;
			*/

			BlockPayload() {}
			BlockPayload(gabac::BitInputStream &instream, uint64_t AU_type, uint64_t descriptor_ID, uint64_t block_payload_size, expandable_array<expandable_array<expandable_array<uint64_t> > > &decoded_symbols) {
				if(descriptor_ID == 11 || descriptor_ID == 15) {
					/*
					encoded_tokentype();
					not necessary just yet
					*/
					// note: there is code missing here, TODO
					readUnsigned(instream, 8*block_payload_size);
				}
				else {
					/*
					uint64_t bits_read_count = 0;
					encoded_descriptor_sequences = EncodedDescriptorSequences(instream, AU_type, descriptor_ID, block_payload_size, bits_read_count, decoded_symbols);
					*/
					// note: properly intergrate GABAC, TODO
					readUnsigned(instream, 8*block_payload_size);
				}
				//readUnsigned(instream, instream.getNumBitsUntilByteAligned());
			}
		} block_payload;

		Block() {}
		Block(gabac::BitInputStream &instream, uint64_t AU_type, expandable_array<expandable_array<expandable_array<uint64_t> > > &decoded_symbols) {
			block_header = BlockHeader(instream);
			std::cout << "block_header read successfully" << std::endl;
			std::cout << "| descriptor_ID: " << block_header.descriptor_ID << std::endl;
			std::cout << "| block_payload_size: " << block_header.block_payload_size << std::endl;
			block_payload = BlockPayload(instream, AU_type, block_header.descriptor_ID, block_header.block_payload_size, decoded_symbols);
		}
	} *block;

	AccessUnit() {}
	AccessUnit(gabac::BitInputStream &instream, std::vector<ParameterSet> collection) {
		access_unit_header = AccessUnitHeader(instream, collection);
		std::cout << "access_unit_header read successfully" << std::endl;
		block = new Block[access_unit_header.num_blocks];

		for(size_t i=0; i<access_unit_header.num_blocks; ++i)
			block[i] = Block(instream, access_unit_header.AU_type, decoded_symbols);


		// read decoded_symbols

		long long descID;
		unsigned long long subseqID, num_symbols, val;

		while(fscanf(fin, "%lld", &descID) != EOF && descID != -1) {
			fscanf(fin, "%llu %llu", &subseqID, &num_symbols);

			std::cout << "| descID, subseqID, num_symbols: " << descID << ", " << subseqID << ", " << num_symbols << " > ";

			for(size_t i=0; i<num_symbols; ++i) {
				fscanf(fin, "%llu", &val);
				decoded_symbols[descID][subseqID][i] = val;
				std::cout << val << " ";
			}

			std::cout << std::endl;
		}
	}
};

#endif