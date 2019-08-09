#include <map>
#include <iostream>
#include <vector>
#include "read.h"
#include "gabac-subset/bit_input_stream.h"
#include "gabac-subset/data_block.h"
#include "gabac-subset/block_stepper.h"

#ifndef DATAUNIT_H
#define DATAUNIT_H

struct DataUnit {
	uint64_t data_unit_type;
	uint64_t data_unit_size;

	RawReference raw_reference;
	ParameterSet parameter_set;
	AccessUnit access_unit;

	DataUnit() {}
	DataUnit(gabac::BitInputStream &instream, std::vector<ParameterSet> &collection) {
		data_unit_type = readUnsigned(instream, 8);

		if(data_unit_type == 0) {
			data_unit_size = readUnsigned(instream, 64);
			raw_reference = RawReference(instream);
		}
		else if(data_unit_type == 1) {
			readUnsigned(instream, 10);
			data_unit_size = readUnsigned(instream, 22);
			parameter_set = ParameterSet(instream);
			collection.push_back(parameter_set);
		} 
		else if(data_unit_type == 2) {
			readUnsigned(instream, 3);
			data_unit_size = readUnsigned(instream, 29);
			access_unit = AccessUnit(instream, collection);
		} 
		else {
			/* skip data unit */
		}
	}
};

#endif