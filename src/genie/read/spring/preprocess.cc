/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "preprocess.h"
#include <genie/core/record/record.h>
#include <genie/util/drain.h>
#include <genie/util/ordered-section.h>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include "params.h"
#include "util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

// ---------------------------------------------------------------------------------------------------------------------

void Preprocessor::setup(const std::string &wdir, size_t num_thr, bool paired_end) {
    cp.preserve_id = true;
    cp.preserve_quality = true;
    cp.num_thr = num_thr;
    working_dir = wdir;
    used = false;

    lock.reset();

    cp.paired_end = paired_end;
    cp.ureads_flag = false;
    cp.num_reads = 0;
    cp.num_reads_clean[0] = 0;
    cp.num_reads_clean[1] = 0;
    cp.max_readlen = 0;
    cp.num_reads_per_block = NUM_READS_PER_BLOCK;
    cp.num_blocks = 0;
    // generate random temp directory in the working directory

    while (true) {
        std::string random_str = "tmp." + spring::random_string(10);
        temp_dir = working_dir + "/" + random_str + '/';
        if (!ghc::filesystem::exists(temp_dir)) break;
    }
    UTILS_DIE_IF(!ghc::filesystem::create_directory(temp_dir), "Cannot create temporary directory.");
    std::cout << "Temporary directory: " << temp_dir << "\n";

    outfileclean[0] = temp_dir + "/input_clean_1.dna";
    outfileclean[1] = temp_dir + "/input_clean_2.dna";
    outfileN[0] = temp_dir + "/input_N.dna";
    outfileN[1] = temp_dir + "/input_N.dna.2";
    outfileorderN[0] = temp_dir + "/read_order_N.bin";
    outfileorderN[1] = temp_dir + "/read_order_N.bin.2";
    outfileid = temp_dir + "/id_1";
    outfilequality[0] = temp_dir + "/quality_1";
    outfilequality[1] = temp_dir + "/quality_2";

    for (int j = 0; j < 2; j++) {
        if (j == 1 && !cp.paired_end) continue;
        fout_clean[j].open(outfileclean[j], std::ios::binary);
        fout_N[j].open(outfileN[j], std::ios::binary);
        fout_order_N[j].open(outfileorderN[j], std::ios::binary);
        if (cp.preserve_quality) fout_quality[j].open(outfilequality[j]);
    }
    if (cp.preserve_id) fout_id.open(outfileid);
}

// ---------------------------------------------------------------------------------------------------------------------

void Preprocessor::preprocess(core::record::Chunk &&t, const util::Section &id) {
    core::record::Chunk data = std::move(t);

    util::OrderedSection lsec(&lock, id);
    used = true;
    stats.add(data.getStats());

    UTILS_DIE_IF(
        data.getData().front().getNumberOfTemplateSegments() * data.getData().size() + cp.num_reads > MAX_NUM_READS,
        "Too many reads");

    size_t rec_index = 0;
    for (auto &rec : data.getData()) {
        UTILS_DIE_IF(rec.getSegments().size() != ((size_t)cp.paired_end + 1), "Number of segments differs");
        size_t seg_index = 0;
        for (auto &seq : rec.getSegments()) {
            UTILS_DIE_IF(seq.getSequence().size() > MAX_READ_LEN, "Too long read length");
            cp.max_readlen = std::max(cp.max_readlen, (uint32_t)seq.getSequence().length());
            if (seq.getSequence().find('N') != std::string::npos) {
                write_dnaN_in_bits(seq.getSequence(),fout_N[seg_index]);
                uint32_t pos_N = cp.num_reads + rec_index;
                fout_order_N[seg_index].write((char *)&pos_N, sizeof(uint32_t));
            } else {
                write_dna_in_bits(seq.getSequence(),fout_clean[seg_index]);
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

    UTILS_DIE_IF(cp.num_reads == 0, "No reads found.");
}

// ---------------------------------------------------------------------------------------------------------------------

void Preprocessor::finish(size_t id) {
    if (!used) {
        return;
    }
    util::Section sec{id, 0, true};
    util::OrderedSection lsec(&lock, sec);
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
        std::ofstream fout_N_PE(outfileN[0], std::ios::app|std::ios::binary);
        std::ifstream fin_N_PE(outfileN[1], std::ios::binary);
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

// ---------------------------------------------------------------------------------------------------------------------

core::stats::PerfStats &Preprocessor::getStats() { return stats; }

// ---------------------------------------------------------------------------------------------------------------------

Preprocessor::~Preprocessor() {
    if (!used) {
        for (int j = 0; j < 2; j++) {
            if (j == 1 && !cp.paired_end) continue;
            fout_clean[j].close();
            fout_N[j].close();
            fout_order_N[j].close();
            if (cp.preserve_quality) fout_quality[j].close();
        }
        if (cp.preserve_id) fout_id.close();

        for (int j = 0; j < 2; j++) {
            if (j == 1 && !cp.paired_end) continue;
            ghc::filesystem::remove(outfileclean[j]);
            ghc::filesystem::remove(outfileN[j]);
            ghc::filesystem::remove(outfileorderN[j]);
            if (cp.preserve_quality) ghc::filesystem::remove(outfilequality[j]);
        }
        if (cp.preserve_id) ghc::filesystem::remove(outfileid);

        ghc::filesystem::remove(temp_dir);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Preprocessor::skip(const util::Section &id) { util::OrderedSection sec(&lock, id); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
