#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

#include <array>
#include <cmath>  // abs
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>

#include "compress-ureads.h"
#include "generate-read-streams.h"
#include "reorder-compress-quality-id.h"
#include "util.h"
#include "params.h"
#include "util.h"

#include <utils/fastq-file-reader.h>

namespace spring {

  std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>
  compress_ureads(utils::FastqFileReader *fastqFileReader1,
                  utils::FastqFileReader *fastqFileReader2,
                  const std::string &temp_dir, compression_params &cp,
                  dsg::StreamSaver& st) {

        std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> descriptorFilesPerAU;

        utils::FastqFileReader *fastqFileReader[2] = {fastqFileReader1, fastqFileReader2};
        std::string basedir = temp_dir;
        std::string outfileid = basedir + "/id_1";
        std::string outfilequality = basedir + "/quality_1";

        std::vector<utils::FastqRecord> fastqRecords[2];

        int number_of_record_segments = cp.paired_end ? 2 : 1;
        uint32_t max_readlen = 0;
        uint64_t num_reads = 0;
        uint32_t num_reads_per_block;
        num_reads_per_block = cp.num_reads_per_block;

        int num_threads = cp.num_thr;

        uint64_t num_reads_per_step = (uint64_t) num_threads * num_reads_per_block;
        std::string *id_array_1 = new std::string[num_reads_per_step];
        uint32_t num_blocks_done = 0;

        while (true) {
            bool done = false;
            uint32_t num_reads_read[2] = {0, 0};
            // read lines from the file(s)
            for (int j = 0; j < number_of_record_segments; j++) {
                num_reads_read[j] = fastqFileReader[j]->readRecords(num_reads_per_step, &fastqRecords[j]);
            }
            // basic checks to determine if we are done
            if (cp.paired_end)
                if (num_reads_read[0] != num_reads_read[1])
                    throw std::runtime_error(
                            "Number of reads in paired files do not match.");
            if (num_reads + num_reads_read[0] + num_reads_read[1] > MAX_NUM_READS) {
                std::cerr << "Max number of reads allowed is " << MAX_NUM_READS << "\n";
                throw std::runtime_error("Too many reads.");
            }
            if (num_reads_read[0] < num_reads_per_step) done = true;

            if (num_reads_read[0] != 0) {
                // parallel portion starts, includes generation of streams and their compression
#ifdef GENIE_USE_OPENMP
#pragma omp parallel for ordered num_threads(cp.num_thr) schedule(dynamic)
#endif
                for (size_t tid = 0; tid < (size_t)cp.num_thr; tid++) {
                        size_t block_num_thr = num_blocks_done + tid;
                        bool done_loop = false;
                        if (tid * num_reads_per_block >= num_reads_read[0]) done_loop = true;
                        uint32_t num_reads_thr = std::min((size_t) num_reads_read[0],
                                                          (tid + 1) * num_reads_per_block) -
                                                 tid * num_reads_per_block;
                        if (!done_loop) {
                            // check if reads and qualities have equal lengths (and more such checks)
                            for (int j = 0; j < number_of_record_segments; j++) {
                                for (uint32_t i = tid * num_reads_per_block;
                                     i < tid * num_reads_per_block + num_reads_thr; i++) {
                                    size_t len = fastqRecords[j][i].sequence.length();
                                    if (len == 0)
                                        throw std::runtime_error("Read of length 0 detected.");
                                    if (len > MAX_READ_LEN_UREADS) {
                                        std::cerr << "Max read length in ureads mode is "
                                                  << MAX_READ_LEN_UREADS << ", but found read of length " << len
                                                  << "\n";
                                        throw std::runtime_error(
                                                "Too long read length");
                                    }
                                    if (cp.preserve_quality && (fastqRecords[j][i].qualityScores.length() != len))
                                        throw std::runtime_error(
                                                "Read length does not match quality length.");
                                    if (cp.preserve_id && (fastqRecords[j][i].title.length() == 0) && j == 0)
                                        throw std::runtime_error("Identifier of length 0 detected.");
                                    if (cp.preserve_id && j == 0) {
                                        id_array_1[i] = fastqRecords[j][i].title;
                                    }
                                }
                            }

                            // generate and compress subsequences
                            subseq_data subseqData;
                            subseqData.block_num = block_num_thr;
                            for (auto arr : subseq_indices) {
                                subseqData.subseq_vector[arr[0]][arr[1]] = std::vector<int64_t>();
                                subseqData.subseq_vector[arr[0]][arr[1]].clear();
                            }

                            // char_to_int
                            int64_t char_to_int[128];
                            char_to_int[(uint8_t) 'A'] = 0;
                            char_to_int[(uint8_t) 'C'] = 1;
                            char_to_int[(uint8_t) 'G'] = 2;
                            char_to_int[(uint8_t) 'T'] = 3;
                            char_to_int[(uint8_t) 'N'] = 4;

                            for (uint32_t i = tid * num_reads_per_block;
                                 i < tid * num_reads_per_block + num_reads_thr; i++) {
                                for (int j = 0; j < number_of_record_segments; j++) {
                                    size_t rlen = fastqRecords[j][i].sequence.length();
                                    max_readlen = std::max((size_t)max_readlen, rlen);
                                    subseqData.subseq_vector[7][0].push_back(rlen - 1); // rlen
                                    for (size_t k = 0; k < rlen; k++)
                                        subseqData.subseq_vector[6][0].push_back(char_to_int[(uint8_t) fastqRecords[j][i].sequence[k]]); // rlen
                                }
                            }

                            compress_subseqs(&subseqData, st);

                            // quality compression
                            std::string outfile_name_quality =
                                    "quality_1." +
                                    std::to_string(block_num_thr);

                            gabac::DataBlock qualityBuffer(0, 1);
                            if (cp.preserve_quality){
                                std::string buffer;
                                for (uint32_t i = tid * num_reads_per_block;
                                     i < tid * num_reads_per_block + num_reads_thr; i++) {
                                    for (int j = 0; j < number_of_record_segments; j++)
                                        buffer += fastqRecords[j][i].qualityScores;
                                }
                                qualityBuffer = gabac::DataBlock(&buffer);
                                st.compress(outfile_name_quality, &qualityBuffer);
                            }

                            // compress ids
                            dsg::AcessUnitStreams streams;
                            std::string outfile_name_id =
                                    "id_1." +
                                    std::to_string(block_num_thr);
                            if (cp.preserve_id) {

                                std::vector<int64_t> tokens[128][8];
                                generate_read_id_tokens(id_array_1 + tid * num_reads_per_block, num_reads_thr, tokens);
                                for (int i = 0; i < 128; i++) {
                                    for (int j = 0; j < 8; j++) {
                                        if (!tokens[i][j].empty()) {
                                            std::string outfile_name_i_j =
                                                    outfile_name_id.substr(outfile_name_id.find_last_of('/') + 1) + "." +
                                                    std::to_string(i) +
                                                    "." + std::to_string(j);
                                            streams.streams[i][j] = gabac::DataBlock(&tokens[i][j]);

                                            st.compress(outfile_name_i_j, &streams.streams[i][j]);
                                        }
                                    }
                                }
                            }

#ifdef GENIE_USE_OPENMP
#pragma omp ordered
#endif
                            {
                                pack_subseqs(&subseqData, st, &descriptorFilesPerAU);
                                if (cp.preserve_quality) {
                                    pack_qual(outfile_name_quality, st, &qualityBuffer);
                                }
                                if (cp.preserve_id) {
                                    pack_id(outfile_name_id, st, &streams);
                                }
                            }
                        } // if (!done_loop)
                    }    // omp parallel for
                    num_reads += num_reads_read[0] + num_reads_read[1];
                }  // if (num_reads_read[0] != 0)
                if (done) break;
                num_blocks_done += num_threads;
            } // while(true)

        delete[] id_array_1;

        if (num_reads == 0) throw std::runtime_error("No reads found.");

        cp.num_reads = num_reads;
        cp.max_readlen = max_readlen;

        std::cout << "Max Read length: " << cp.max_readlen << "\n";
        std::cout << "Total number of reads: " << cp.num_reads << "\n";
        return descriptorFilesPerAU;
    }

}  // namespace spring
