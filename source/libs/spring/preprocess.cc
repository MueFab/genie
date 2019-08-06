#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "params.h"
#include "util.h"
#include "preprocess.h"

#include <utils/fastq-file-reader.h>

namespace spring {

    void preprocess(utils::FastqFileReader *fastqFileReader1,
                    utils::FastqFileReader *fastqFileReader2,
                    const std::string &temp_dir, compression_params &cp) {
        utils::FastqFileReader *fastqFileReader[2] = {fastqFileReader1, fastqFileReader2};
        std::string outfileclean[2];
        std::string outfileN[2];
        std::string outfileorderN[2];
        std::string outfileid;
        std::string outfilequality[2];
        std::string outfilereadlength[2];
        std::string basedir = temp_dir;
        outfileclean[0] = basedir + "/input_clean_1.dna";
        outfileclean[1] = basedir + "/input_clean_2.dna";
        outfileN[0] = basedir + "/input_N.dna";
        outfileN[1] = basedir + "/input_N.dna.2";
        outfileorderN[0] = basedir + "/read_order_N.bin";
        outfileorderN[1] = basedir + "/read_order_N.bin.2";
        outfileid = basedir + "/id_1";
        outfilequality[0] = basedir + "/quality_1";
        outfilequality[1] = basedir + "/quality_2";

        std::ofstream fout_clean[2];
        std::ofstream fout_N[2];
        std::ofstream fout_order_N[2];
        std::ofstream fout_id;
        std::ofstream fout_quality[2];

        std::vector<utils::FastqRecord> fastqRecords;
        for (int j = 0; j < 2; j++) {
            if (j == 1 && !cp.paired_end) continue;
            fout_clean[j].open(outfileclean[j]);
            fout_N[j].open(outfileN[j]);
            fout_order_N[j].open(outfileorderN[j], std::ios::binary);
            if (cp.preserve_quality) fout_quality[j].open(outfilequality[j]);
        }
        if (cp.preserve_id) fout_id.open(outfileid);

        uint32_t max_readlen = 0;
        uint64_t num_reads[2] = {0, 0};
        uint64_t num_reads_clean[2] = {0, 0};
        uint32_t num_reads_per_block;
        num_reads_per_block = cp.num_reads_per_block;

//  char *quality_binning_table = new char[128];
//  if (cp.ill_bin_flag) generate_illumina_binning_table(quality_binning_table);
//  if (cp.bin_thr_flag) generate_binary_binning_table(quality_binning_table, cp.bin_thr_thr, cp.bin_thr_high, cp.bin_thr_low);

        //
        // Disable parallelism for this loop.
        // All real work is done by the master thread, and the worker threads
        // just sit at an OpenMP barrier.  Running on a single thread speeds
        // things up slightly.
        //
        // int num_threads = cp.num_thr;
        int num_threads = 1;
#undef GENIE_USE_OPENMP

        uint64_t num_reads_per_step = (uint64_t) num_threads * num_reads_per_block;
        bool *read_contains_N_array = new bool[num_reads_per_step];
        uint32_t *read_lengths_array = new uint32_t[num_reads_per_step];
        uint32_t num_blocks_done = 0;

        while (true) {
            bool done[2] = {true, true};
            for (int j = 0; j < 2; j++) {
                if (j == 1 && !cp.paired_end) continue;
                done[j] = false;
                uint32_t num_reads_read = fastqFileReader[j]->readRecords(num_reads_per_step, &fastqRecords);
                if (num_reads_read < num_reads_per_step) done[j] = true;
                if (num_reads_read == 0) continue;
                if (num_reads[0] + num_reads[1] + num_reads_read > MAX_NUM_READS) {
                    std::cerr << "Max number of reads allowed is " << MAX_NUM_READS << "\n";
                    throw std::runtime_error("Too many reads.");
                }
#ifdef GENIE_USE_OPENMP
#pragma omp parallel num_threads(cp.num_thr)
#endif
                {
                    bool done_loop = false;
#ifdef GENIE_USE_OPENMP
                    uint64_t tid = omp_get_thread_num();
#else
                    uint64_t tid = 0;
#endif
                    if (tid * num_reads_per_block >= num_reads_read) done_loop = true;
                    uint32_t num_reads_thr = std::min((uint64_t) num_reads_read,
                                                      (tid + 1) * num_reads_per_block) -
                                             tid * num_reads_per_block;
                    if (!done_loop) {
                        // check if reads and qualities have equal lengths
                        for (uint32_t i = tid * num_reads_per_block;
                             i < tid * num_reads_per_block + num_reads_thr; i++) {
                            size_t len = fastqRecords[i].sequence.length();
                            if (len == 0)
                                throw std::runtime_error("Read of length 0 detected.");
                            if (len > MAX_READ_LEN) {
                                std::cerr << "Max read length without ureads mode is "
                                          << MAX_READ_LEN << ", but found read of length " << len
                                          << "\n";
                                throw std::runtime_error(
                                        "Too long read length");// (please try --long/-l flag).");
                            }
                            if (cp.preserve_quality && (fastqRecords[i].qualityScores.length() != len))
                                throw std::runtime_error(
                                        "Read length does not match quality length.");
                            if (cp.preserve_id && (fastqRecords[i].title.length() == 0) && j == 0)
                                throw std::runtime_error("Identifier of length 0 detected.");
                            read_lengths_array[i] = (uint32_t) len;

                            read_contains_N_array[i] =
                                    (fastqRecords[i].sequence.find('N') != std::string::npos);
                        }
                        // apply binning (if asked to do so)
//          if (cp.preserve_quality && (cp.ill_bin_flag || cp.bin_thr_flag))
//            quantize_quality(quality_array + tid * num_reads_per_block,
//                             num_reads_thr, quality_binning_table);

                        // apply qvz quantization (if flag specified and order preserving)
//	  if(cp.preserve_quality && cp.qvz_flag && cp.preserve_order)
//            quantize_quality_qvz(quality_array + tid * num_reads_per_block, num_reads_thr, read_lengths_array + tid * num_reads_per_block, cp.qvz_ratio);
                    }  // if(!done)
                }    // omp parallel
                // write reads and read_order_N to respective files
                for (uint32_t i = 0; i < num_reads_read; i++) {
                    if (!read_contains_N_array[i]) {
                        fout_clean[j] << fastqRecords[i].sequence << "\n";
                        num_reads_clean[j]++;
                    } else {
                        uint32_t pos_N = num_reads[j] + i;
                        fout_order_N[j].write((char *) &pos_N, sizeof(uint32_t));
                        fout_N[j] << fastqRecords[i].sequence << "\n";
                    }
                }
//          // write qualities to file
                for (uint32_t i = 0; i < num_reads_read; i++)
                    fout_quality[j] << fastqRecords[i].qualityScores << "\n";
                // write ids to file
                if (j == 0)
                    for (uint32_t i = 0; i < num_reads_read; i++)
                        fout_id << fastqRecords[i].title << "\n";
                num_reads[j] += num_reads_read;
                max_readlen =
                        std::max(max_readlen,
                                 *(std::max_element(read_lengths_array,
                                                    read_lengths_array + num_reads_read)));
            }
            if (cp.paired_end)
                if (num_reads[0] != num_reads[1])
                    throw std::runtime_error(
                            "Number of reads in paired files do not match.");
            if (done[0] && done[1]) break;
            num_blocks_done += num_threads;
        }

        delete[] read_contains_N_array;
        delete[] read_lengths_array;
//  delete[] quality_binning_table;
        // close files
        for (int j = 0; j < 2; j++) {
            if (j == 1 && !cp.paired_end) continue;
            fout_clean[j].close();
            fout_N[j].close();
            fout_order_N[j].close();
            if (cp.preserve_quality) fout_quality[j].close();
        }
        if (cp.preserve_id) fout_id.close();

        if (num_reads[0] == 0) throw std::runtime_error("No reads found.");

        if (cp.paired_end) {
            // merge input_N and input_order_N for the two files
            std::ofstream fout_N_PE(outfileN[0], std::ios::app);
            std::ifstream fin_N_PE(outfileN[1]);
            fout_N_PE << fin_N_PE.rdbuf();
            fout_N_PE.close();
            fin_N_PE.close();
            remove(outfileN[1].c_str());
            std::ofstream fout_order_N_PE(outfileorderN[0],
                                       std::ios::app | std::ios::binary);
            std::ifstream fin_order_N(outfileorderN[1], std::ios::binary);
            uint32_t num_N_file_2 = num_reads[1] - num_reads_clean[1];
            uint32_t order_N;
            for (uint32_t i = 0; i < num_N_file_2; i++) {
                fin_order_N.read((char *) &order_N, sizeof(uint32_t));
                order_N += num_reads[0];
                fout_order_N_PE.write((char *) &order_N, sizeof(uint32_t));
            }
            fin_order_N.close();
            fout_order_N_PE.close();
            remove(outfileorderN[1].c_str());
        }

        cp.num_reads = num_reads[0] + num_reads[1];
        cp.num_reads_clean[0] = num_reads_clean[0];
        cp.num_reads_clean[1] = num_reads_clean[1];
        cp.max_readlen = max_readlen;

        std::cout << "Max Read length: " << cp.max_readlen << "\n";
        std::cout << "Total number of reads: " << cp.num_reads << "\n";

        std::cout << "Total number of reads without N: "
                  << cp.num_reads_clean[0] + cp.num_reads_clean[1] << "\n";
    }

}  // namespace spring
