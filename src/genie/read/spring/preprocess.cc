/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

#include <genie/core/parameter/parameter_set.h>
#include <genie/core/record/record.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include "params.h"
#include "preprocess.h"
#include "spring.h"
#include "util.h"

namespace genie {
namespace read {
namespace spring {


void Preprocessor::preprocess(core::record::Chunk &&t, size_t id) {
    core::record::Chunk data = std::move(t);

    if(!init) {
        init = true;
        cp.paired_end = data.front().getSegments().size() == 2;
        UTILS_DIE_IF(data.front().getSegments().size() > 2, "Maximum of two segments per read supported");
        cp.ureads_flag = false;
        cp.num_reads = 0;
        cp.num_reads_clean[0] = 0;
        cp.num_reads_clean[1] = 0;
        cp.max_readlen = 0;
        cp.num_reads_per_block = 0;
        cp.num_thr = 1;
        cp.num_blocks = 0;
        for (int j = 0; j < 2; j++) {
            if (j == 1 && !cp.paired_end) continue;
            fout_clean[j].open(outfileclean[j]);
            fout_N[j].open(outfileN[j]);
            fout_order_N[j].open(outfileorderN[j], std::ios::binary);
            if (cp.preserve_quality) fout_quality[j].open(outfilequality[j]);
        }
        if (cp.preserve_id) fout_id.open(outfileid);
    }

    UTILS_DIE_IF(data.front().getNumberOfTemplateSegments() * data.size() + cp.num_reads > MAX_NUM_READS,
                 "Too many reads");

    size_t rec_index = 0;
    for (auto &rec : data) {
        UTILS_DIE_IF(rec.getSegments().size() != ((size_t)cp.paired_end + 1), "Number of segments differs");
        size_t seg_index = 0;
        for (auto &seq : rec.getSegments()) {
            UTILS_DIE_IF(seq.getSequence().size() > MAX_READ_LEN, "Too long read length");
            cp.max_readlen = std::max(cp.max_readlen, (uint32_t)seq.getSequence().length());
            if (seq.getSequence().find('N') != std::string::npos) {
                fout_N[seg_index] << seq.getSequence() << "\n";
                uint32_t pos_N = cp.num_reads + rec_index;
                fout_order_N[seg_index].write((char *)&pos_N, sizeof(uint32_t));
            } else {
                fout_clean[seg_index] << seq.getSequence() << "\n";
                cp.num_reads_clean[seg_index]++;
            }
            if (!seq.getQualities().empty()) {
                fout_quality[seg_index] << seq.getQualities().front() << "\n";
            }
            ++seg_index;
        }
        fout_id << rec.getName() << "\n";
        ++rec_index;
    }
    cp.num_reads += rec_index;
    cp.num_blocks++;

    // ---------------------------------------------------------------------------------------------

    UTILS_DIE_IF(cp.num_reads == 0, "No reads found.");
}

void Preprocessor::finish() {
    for (int j = 0; j < 2; j++) {
        if (j == 1 && !cp.paired_end) continue;
        fout_clean[j].close();
        fout_N[j].close();
        fout_order_N[j].close();
        if (cp.preserve_quality) fout_quality[j].close();
    }
    if (cp.preserve_id) fout_id.close();

    if (cp.paired_end) {
        // merge input_N and input_order_N for the two files
        std::ofstream fout_N_PE(outfileN[0], std::ios::app);
        std::ifstream fin_N_PE(outfileN[1]);
        fout_N_PE << fin_N_PE.rdbuf();
        fout_N_PE.close();
        fin_N_PE.close();
        remove(outfileN[1].c_str());
        std::ofstream fout_order_N_PE(outfileorderN[0], std::ios::app | std::ios::binary);
        std::ifstream fin_order_N(outfileorderN[1], std::ios::binary);
        uint32_t num_N_file_2 = cp.num_reads - cp.num_reads_clean[1];
        uint32_t order_N;
        for (uint32_t i = 0; i < num_N_file_2; i++) {
            fin_order_N.read((char *)&order_N, sizeof(uint32_t));
            order_N += cp.num_reads;
            fout_order_N_PE.write((char *)&order_N, sizeof(uint32_t));
        }
        fin_order_N.close();
        fout_order_N_PE.close();
        remove(outfileorderN[1].c_str());
    }

    cp.num_reads = cp.paired_end ? cp.num_reads * 2 : cp.num_reads;

    std::cout << "Max Read length: " << cp.max_readlen << "\n";
    std::cout << "Total number of reads: " << cp.num_reads << "\n";

    std::cout << "Total number of reads without N: " << cp.num_reads_clean[0] + cp.num_reads_clean[1] << "\n";
}

}  // namespace read
}  // namespace genie
}  // namespace genie