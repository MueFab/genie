#include "decoder.h"
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

int main(int argc, char **argv) {
	assert(argc == 3);

	std::ifstream t(argv[1]);
	std::string file_contents;

	t.seekg(0, std::ios::end);   
	file_contents.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	file_contents.assign((std::istreambuf_iterator<char>(t)),
	           			  std::istreambuf_iterator<char>());

	std::ifstream t1(argv[2]);
	std::string file_contents1;

	t1.seekg(0, std::ios::end);   
	file_contents1.reserve(t1.tellg());
	t1.seekg(0, std::ios::beg);

	file_contents1.assign((std::istreambuf_iterator<char>(t1)),
	           			   std::istreambuf_iterator<char>());

	std::cout << "Files read successfully." << std::endl;
	std::cout << file_contents.size() << std::endl;
	std::cout << file_contents1.size() << std::endl;

	size_t initial_bitcount = 8*file_contents.size();
	
	for(size_t i=0; i<1000; ++i)
		std::cout << file_contents[i];
	std::cout << std::endl;
	
	gabac::DataBlock db(&file_contents);
	gabac::BitInputStream instream(&db);
	std::vector<ParameterSet> collection;
	std::vector<DataUnit> data_units;

	gabac::DataBlock db1(&file_contents1);
	gabac::BitInputStream instream1(&db1);
	RawReference RR(instream1);

	size_t iterations = 0;

	std::cout << "> Bit count: " << initial_bitcount << std::endl;

	fin = fopen("../input/decoded_symbols_small.txt", "r");

	std::cout << "Opened file successfully" << std::endl;
	
	uint64_t previousMappingPos0 = 0;

	while(track(0) < initial_bitcount && iterations < 4) {	
		DataUnit du(instream, collection);

		iterations++;

		data_units.push_back(du);

		if(du.data_unit_type == 1) {
			std::cout << "ParameterSet" << std::endl;
			std::cout << "Bits read so far: " << track(0) << std::endl;

			/*
			std::cout << DUMP(du.data_unit_type) << std::endl;
			std::cout << DUMP(du.data_unit_size) << std::endl;
			std::cout << std::endl;

			std::cout << DUMP(du.parameter_set.parameter_set_ID) << std::endl;
			std::cout << DUMP(du.parameter_set.parent_parameter_set_ID) << std::endl;
			std::cout << std::endl;

			std::cout << DUMP(du.parameter_set.encoding_parameters.dataset_type) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.alphabet_ID) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.read_length) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.number_of_template_segments_minus1) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.max_au_data_unit_size) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.pos_40_bits_flag) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.qv_depth) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.as_depth) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.num_classes) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.num_groups) << std::endl;

			std::cout << DUMP(du.parameter_set.encoding_parameters.multiple_alignments_flag) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.spliced_reads_flag) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.multiple_signature_base) << std::endl;
			std::cout << DUMP(du.parameter_set.encoding_parameters.crps_flag) << std::endl;
			*/
		} else if(du.data_unit_type == 2) {
			std::cout << "AccessUnit" << std::endl;
			std::cout << "Bits read so far: " << track(0) << std::endl;

			/*
			std::cout << du.access_unit.access_unit_header.access_unit_ID << std::endl;
			std::cout << du.access_unit.access_unit_header.num_blocks << std::endl;
			std::cout << du.access_unit.access_unit_header.parameter_set_ID << std::endl;
			std::cout << du.access_unit.access_unit_header.AU_type << std::endl;
			std::cout << du.access_unit.access_unit_header.reads_count << std::endl;
			std::cout << du.access_unit.access_unit_header.mm_threshold << std::endl;
			std::cout << du.access_unit.access_unit_header.mm_count << std::endl;
			std::cout << DUMP(du.access_unit.access_unit_header.ref_sequence_ID) << std::endl;
			std::cout << DUMP(du.access_unit.access_unit_header.ref_start_position) << std::endl;
			std::cout << DUMP(du.access_unit.access_unit_header.ref_end_position) << std::endl;
			std::cout << du.access_unit.access_unit_header.sequence_ID << std::endl;
			std::cout << DUMP(du.access_unit.access_unit_header.AU_start_position) << std::endl;
			std::cout << DUMP(du.access_unit.access_unit_header.AU_end_position) << std::endl;
			std::cout << du.access_unit.access_unit_header.extended_AU_start_position << std::endl;
			std::cout << du.access_unit.access_unit_header.extended_AU_end_position << std::endl;
			std::cout << du.access_unit.access_unit_header.num_signatures << std::endl;
			*/
			std::cout << "---------" << std::endl;
			decode(du.access_unit, collection, RR, previousMappingPos0);
			std::cout << "---------" << std::endl;
		} else if(du.data_unit_type == 0) {
			std::cout << "RawReference" << std::endl;
			
			//std::cout << du.raw_reference.seq_count << std::endl;
		} else std::cout << "Unknown data_unit_type: " << du.data_unit_type << std::endl;
	}

	fclose(fin);

	return 0;
}
