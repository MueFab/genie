#include <iostream>
#include <map>
#include "gabac-subset/bit_input_stream.h"
#include "gabac-subset/block_stepper.h"
#include "gabac-subset/data_block.h"
#include "read.h"

#ifndef RAWREFERENCE_H
#define RAWREFERENCE_H

struct RawReference {
    uint64_t seq_count;
    uint64_t *sequence_ID;

    std::map<uint64_t, uint64_t> seq_start;
    std::map<uint64_t, uint64_t> seq_end;
    std::map<uint64_t, std::string> ref_sequence;

    RawReference() {}
    RawReference(gabac::BitInputStream &instream) {
        std::cout << "RawReference" << std::endl;

        seq_count = readUnsigned(instream, 16);
        sequence_ID = new uint64_t[seq_count];

        for (size_t i = 0; i < seq_count; ++i) {
            sequence_ID[i] = readUnsigned(instream, 16);
            seq_start[sequence_ID[i]] = readUnsigned(instream, 40);
            seq_end[sequence_ID[i]] = readUnsigned(instream, 40);

            std::cout << "| seq_start: " << seq_start[sequence_ID[i]] << std::endl;
            std::cout << "| seq_end: " << seq_end[sequence_ID[i]] << std::endl;

            for (size_t j = 0; j < seq_end[sequence_ID[i]] - seq_start[sequence_ID[i]] + 1; ++j)
                ref_sequence[sequence_ID[i]].push_back((unsigned char)instream.readByte());

            std::cout << "| " << ref_sequence[sequence_ID[i]] << std::endl;
        }
    }
};

#endif