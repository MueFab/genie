#ifdef GENIE_USE_OPENMP

#include <omp.h>

#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "spring-gabac.h"
#include "id-tokenization.h"
#include "params.h"
#include "reorder-compress-quality-id.h"
#include "util.h"

namespace spring {

void reorder_compress_quality_id(const std::string &temp_dir, const compression_params &cp, bool analyze, dsg::StreamSaver *st, const std::vector<std::vector<gabac::EncodingConfiguration>>& configs, util::FastqStats *stats) {
    // Read some parameters
    uint32_t numreads = cp.num_reads;
    int num_thr = cp.num_thr;
    bool preserve_id = cp.preserve_id;
    bool preserve_quality = cp.preserve_quality;
    bool paired_end = cp.paired_end;
    uint32_t num_reads_per_block = cp.num_reads_per_block;

    std::string basedir = temp_dir;

    std::string file_order = basedir + "/read_order.bin";
    std::string file_id = basedir + "/id_1";
    ;
    std::string file_quality[2];
    file_quality[0] = basedir + "/quality_1";
    file_quality[1] = basedir + "/quality_2";
    std::string outfile_quality = "quality_1";

    if (!paired_end) {
        uint32_t *order_array;
        // array containing index mapping position in original fastq to
        // position after reordering
        order_array = new uint32_t[numreads];
        generate_order(file_order, order_array, numreads);

        uint32_t str_array_size = (1 + (numreads / 4 - 1) / num_reads_per_block) * num_reads_per_block;
        ;
        // smallest multiple of num_reads_per_block bigger than numreads/4
        // numreads/4 chosen so that these many qualities/ids can be stored in
        // memory without exceeding the RAM consumption of reordering stage
        std::string *str_array = new std::string[str_array_size];
        // array to load ids and/or qualities into

        if (preserve_quality) {
            std::cout << "Compressing qualities\n";
            uint32_t num_reads_per_file = numreads;
            reorder_compress(file_quality[0], temp_dir, num_reads_per_file, num_thr, num_reads_per_block, str_array,
                             str_array_size, order_array, "quality", analyze, st, configs, stats);
            remove(file_quality[0].c_str());
        }
        if (preserve_id) {
            std::cout << "Compressing ids\n";
            uint32_t num_reads_per_file = numreads;
            reorder_compress(file_id, temp_dir, num_reads_per_file, num_thr, num_reads_per_block, str_array, str_array_size,
                             order_array, "id", analyze, st, configs, stats);
            remove(file_id.c_str());
        }

        delete[] order_array;
        delete[] str_array;

    } else {
        // paired end
        std::string file_order_quality = basedir + "/order_quality.bin";
        std::string file_order_id = basedir + "/order_id.bin";
        std::string file_blocks_quality = basedir + "/blocks_quality.bin";
        std::string file_blocks_id = basedir + "/blocks_id.bin";
        std::vector<uint32_t> block_start, block_end;
        if (preserve_quality) {
            // read block start and end into vector
            read_block_start_end(file_blocks_quality, block_start, block_end);
            // read order into order_array
            uint32_t *order_array = new uint32_t[numreads];
            generate_order(file_order_quality, order_array, numreads);
            uint64_t quality_array_size = numreads / 4 + 3 * num_reads_per_block;
            std::string *quality_array = new std::string[quality_array_size];
            // numreads/4 so that memory consumption isn't too high
            // 3*num_reads_per_block added to ensure that we are done in 4 passes
            // (needed because block sizes are not exactly equal to
            // num_reads_per_block
            reorder_compress_quality_pe(file_quality, outfile_quality, temp_dir, quality_array, quality_array_size, order_array,
                                        block_start, block_end, cp, analyze, st, configs, stats);
            delete[] quality_array;
            delete[] order_array;
            remove(file_quality[0].c_str());
            remove(file_quality[1].c_str());
            block_start.clear();
            block_end.clear();
        }
        if (preserve_id) {
            read_block_start_end(file_blocks_id, block_start, block_end);
            std::string *id_array = new std::string[numreads / 2];
            std::ifstream f_id(file_id);
            for (uint32_t i = 0; i < numreads / 2; i++) std::getline(f_id, id_array[i]);
            reorder_compress_id_pe(id_array, temp_dir, file_order_id, block_start, block_end, file_id, cp, analyze, st, configs, stats);
            delete[] id_array;
            for (uint32_t i = 0; i < block_start.size(); i++) remove((file_order_id + "." + std::to_string(i)).c_str());
            remove(file_id.c_str());
            block_start.clear();
            block_end.clear();
        }
        remove(file_order_quality.c_str());
        remove(file_blocks_quality.c_str());
        remove(file_blocks_quality.c_str());
    }
}

void read_block_start_end(const std::string &file_blocks, std::vector<uint32_t> &block_start,
                          std::vector<uint32_t> &block_end) {
    std::ifstream f_blocks(file_blocks, std::ios::binary);
    uint32_t block_pos_temp;
    f_blocks.read((char *)&block_pos_temp, sizeof(uint32_t));
    while (!f_blocks.eof()) {
        block_start.push_back(block_pos_temp);
        f_blocks.read((char *)&block_pos_temp, sizeof(uint32_t));
        block_end.push_back(block_pos_temp);
        f_blocks.read((char *)&block_pos_temp, sizeof(uint32_t));
    }
    f_blocks.close();
}

void generate_order(const std::string &file_order, uint32_t *order_array, const uint32_t &numreads) {
    std::ifstream fin_order(file_order, std::ios::binary);
    uint32_t order;
    for (uint32_t i = 0; i < numreads; i++) {
        fin_order.read((char *)&order, sizeof(uint32_t));
        order_array[order] = i;
    }
    fin_order.close();
}

void reorder_compress_id_pe(std::string *id_array, const std::string &temp_dir, const std::string &file_order_id,
                            const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                            const std::string &file_name, const compression_params &cp, bool analyze, dsg::StreamSaver *st,
                            const std::vector<std::vector<gabac::EncodingConfiguration>>& configs,
                            util::FastqStats *stats) {
    const std::string id_desc_prefix = temp_dir + "/id_streams.";
    (void) cp;

    if (analyze) {
        if (st == NULL) {
            throw std::runtime_error("No config in analyze mode.");
        }
        uint32_t block_num = block_start.size() / 2;
        std::map<std::string, gabac::DataBlock> str;
        std::map<std::string, std::string> strname;
        dsg::AcessUnitStreams AUStreams;
        std::ifstream f_order_id(file_order_id + "." + std::to_string(block_num), std::ios::binary);
        std::vector<int64_t> tokens[128][6];
        std::string *id_array_block = new std::string[block_end[block_num] - block_start[block_num]];
        uint32_t index;
        for (uint32_t j = block_start[block_num]; j < block_end[block_num]; j++) {
            f_order_id.read((char *)&index, sizeof(uint32_t));
            id_array_block[j - block_start[block_num]] = id_array[index];
        }
        generate_read_id_tokens(id_array_block, block_end[block_num] - block_start[block_num], tokens);
        std::string outfile_name = file_name + "." + std::to_string(block_num);
        for (int i = 0; i < 128; i++) {
            for (int j = 0; j < 6; j++) {
                if (!tokens[i][j].empty()) {
                    std::string outfile_name_i_j = outfile_name.substr(outfile_name.find_last_of('/') + 1) + "." +
                                                   std::to_string(i) + "." + std::to_string(j);
                    auto block = gabac::DataBlock(&tokens[i][j]);
                    auto confname = st->getConfigName(outfile_name_i_j);
                    if (str[confname].getRawSize() < block.getRawSize()) {
                        str[confname].swap(&block);
                        strname[confname] = outfile_name_i_j;
                    }
                }
            }
        }
#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(cp.num_thr)
#else
        (void)cp;       // Suppress unused parameter warning
#endif
        for (size_t i = 0; i < str.size(); ++i) {
            auto it = str.begin();
            std::advance(it, i);
            auto &p = *it;
            st->analyze(strname[p.first], &p.second);
        }
        st->reloadConfigSet();
        f_order_id.close();
        delete[] id_array_block;
    }

    uint64_t size = 0;

#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(cp.num_thr) schedule(dynamic) reduction(+:size)
#endif
    for (uint64_t block_num = 0; block_num < block_start.size(); block_num++) {
        std::ifstream f_order_id(file_order_id + "." + std::to_string(block_num), std::ios::binary);
        std::string *id_array_block = new std::string[block_end[block_num] - block_start[block_num]];
        uint32_t index;
        for (uint32_t j = block_start[block_num]; j < block_end[block_num]; j++) {
            f_order_id.read((char *)&index, sizeof(uint32_t));
            id_array_block[j - block_start[block_num]] = id_array[index];
        }
        auto raw_data = generate_empty_raw_data();
        std::vector<std::vector<std::vector<gabac::DataBlock>>> generated_streams =
            create_default_streams();
        generate_read_id_tokens(id_array_block, block_end[block_num] - block_start[block_num], raw_data[15]);
        std::string name = file_name + "." + std::to_string(block_num);
        for (int i = 0; i < 128; i++) {
            for (int j = 0; j < 6; j++) {
                if (raw_data[15][6 * i + j].size() == 0) {
                    continue;
                }
                const gabac::EncodingConfiguration *config;
                if ((st == NULL) || ((config = st->getConfig(
                  name.substr(name.find_last_of('/') + 1) + "."
                  + std::to_string(i) + "." + std::to_string(j)))
                  == NULL)) {
                    config = &configs[15][0];
                }
                // FIXME - &configs[15][0]is loop invariant,
                // but getConfig(name) is not.  suspicious.
                gabac_compress(*config, &raw_data[15][6 * i + j],
                               &generated_streams[15][6 * i + j]);
            }
        }
        std::string file_to_save_streams = id_desc_prefix + std::to_string(block_num);
        size += write_streams_to_file(generated_streams, file_to_save_streams, id_descriptors);

        f_order_id.close();
        delete[] id_array_block;
    }

    if (stats->enabled) {
        stats->cmprs_id_sz += size;
        stats->cmprs_total_sz += size;
    }
}

void reorder_compress_quality_pe(std::string file_quality[2],
                                 const std::string &outfile_quality,
                                 const std::string &temp_dir,
                                 std::string *quality_array, const uint64_t &quality_array_size, uint32_t *order_array,
                                 const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                                 const compression_params &cp, bool analyze, dsg::StreamSaver *st,
                                 const std::vector<std::vector<gabac::EncodingConfiguration>>& configs,
                                 util::FastqStats *stats) {
    const std::string quality_desc_prefix = temp_dir + "/quality_streams.";
    uint32_t start_block_num = 0;
    uint32_t end_block_num = 0;
    bool analysis_done = false;
    while (true) {
        // first find blocks to read from file
        if (start_block_num >= block_start.size()) break;
        end_block_num = start_block_num;
        while (end_block_num < block_end.size()) {
            if (block_end[end_block_num] - block_start[start_block_num] > quality_array_size) break;
            end_block_num++;
        }
        std::string temp_str;
        for (int j = 0; j < 2; j++) {
            std::ifstream f_in(file_quality[j]);
            uint32_t num_reads_offset = j * (cp.num_reads / 2);
            for (uint32_t i = 0; i < cp.num_reads / 2; i++) {
                std::getline(f_in, temp_str);
                if (order_array[i + num_reads_offset] >= block_start[start_block_num] &&
                    order_array[i + num_reads_offset] < block_end[end_block_num - 1])
                    quality_array[order_array[i + num_reads_offset] - block_start[start_block_num]] = temp_str;
            }
        }

        if (analyze && !analysis_done) {
            if (st == NULL) {
                throw std::runtime_error("No config in analyze mode.");
            }
            analysis_done = true;

            uint64_t block_num = (end_block_num + start_block_num) / 2;
            std::string outfile_name = outfile_quality + "." + std::to_string(block_num);
            std::string buffer;
            for (uint32_t i = block_start[block_num]; i < block_end[block_num]; i++)
                buffer += quality_array[i - block_start[start_block_num]];
            gabac::DataBlock d(&buffer);
            st->analyze(outfile_name, &d);
            st->reloadConfigSet();
        }

        uint64_t size = 0;

#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(cp.num_thr) schedule(dynamic) reduction(+:size)
#endif
        for (uint64_t block_num = start_block_num; block_num < end_block_num; block_num++) {
            auto raw_data = generate_empty_raw_data();
            std::vector<std::vector<std::vector<gabac::DataBlock>>> generated_streams =
                create_default_streams();
            for (uint32_t i = block_start[block_num]; i < block_end[block_num]; i++)
                for (size_t pos_in_read = 0; pos_in_read < quality_array[i - block_start[start_block_num]].size(); pos_in_read++)
                  raw_data[14][2].push_back((uint8_t)quality_array[i - block_start[start_block_num]][pos_in_read] - 33);  // quality

            std::string name = outfile_quality + "." + std::to_string(block_num);
            for (size_t subseq = 0; subseq < 3; subseq++) {
                if (raw_data[14][subseq].size() == 0) {
                    continue;
                }
                const gabac::EncodingConfiguration *config;
                if ((st == NULL) || ((config = st->getConfig(name)) == NULL)) {
                    config = &configs[14][subseq];
                }
                gabac_compress(*config, &raw_data[14][subseq],
                               &generated_streams[14][subseq]);
            }
            std::string file_to_save_streams = quality_desc_prefix + std::to_string(block_num);
            write_streams_to_file(generated_streams, file_to_save_streams, quality_descriptors);
        }
        start_block_num = end_block_num;

        if (stats->enabled) {
            stats->cmprs_qual_sz += size;
            stats->cmprs_total_sz += size;
        }
    }
}

void reorder_compress(const std::string &file_name, const std::string &temp_dir, const uint32_t &num_reads_per_file, const int &num_thr,
                      const uint32_t &num_reads_per_block, std::string *str_array, const uint32_t &str_array_size,
                      uint32_t *order_array, const std::string &mode,
                      bool analyze, dsg::StreamSaver *st,
                      const std::vector<std::vector<gabac::EncodingConfiguration>>& configs,
                      util::FastqStats *stats) {
    const std::string id_desc_prefix = temp_dir + "/id_streams.";
    const std::string quality_desc_prefix = temp_dir + "/quality_streams.";
    for (uint32_t ndex = 0; ndex <= num_reads_per_file / str_array_size; ndex++) {
        uint32_t num_reads_bin = str_array_size;
        if (ndex == num_reads_per_file / str_array_size) num_reads_bin = num_reads_per_file % str_array_size;
        if (num_reads_bin == 0) break;
        uint32_t start_read_bin = ndex * str_array_size;
        uint32_t end_read_bin = ndex * str_array_size + num_reads_bin;
        // Read the file and pick up lines corresponding to this bin
        std::ifstream f_in(file_name);
        std::string temp_str;
        for (uint32_t i = 0; i < num_reads_per_file; i++) {
            std::getline(f_in, temp_str);
            if (order_array[i] >= start_read_bin && order_array[i] < end_read_bin)
                str_array[order_array[i] - start_read_bin] = temp_str;
        }
        f_in.close();
        uint64_t blocks = uint64_t(std::ceil(float(num_reads_bin) / num_reads_per_block));

        if (analyze) {
            if (st == NULL) {
                throw std::runtime_error("No config in analyze mode.");
            }
            analyze = false;
            uint64_t block_num = blocks / 2;
            dsg::AcessUnitStreams streams;
            uint64_t block_num_offset = start_read_bin / num_reads_per_block;
            uint64_t start_read_num = block_num * num_reads_per_block;
            uint64_t end_read_num = (block_num + 1) * num_reads_per_block;
            if (end_read_num >= num_reads_bin) {
                end_read_num = num_reads_bin;
            }
            uint32_t num_reads_block = (uint32_t)(end_read_num - start_read_num);
            std::string outfile_name =
                file_name.substr(file_name.find_last_of('/') + 1) + "." + std::to_string(block_num_offset + block_num);

            gabac::DataBlock qualityBuffer(0, 1);

            if (mode == "id") {
                std::vector<int64_t> tokens[128][6];
                generate_read_id_tokens(str_array + start_read_num, num_reads_block, tokens);
                std::map<std::string, gabac::DataBlock> str;
                std::map<std::string, std::string> strname;
                for (int i = 0; i < 128; i++) {
                    for (int j = 0; j < 6; j++) {
                        if (!tokens[i][j].empty()) {
                            std::string outfile_name_i_j = outfile_name.substr(outfile_name.find_last_of('/') + 1) +
                                                           "." + std::to_string(i) + "." + std::to_string(j);
                            auto confname = st->getConfigName(outfile_name_i_j);
                            auto block = gabac::DataBlock(&tokens[i][j]);
                            if (str[confname].getRawSize() < block.getRawSize()) {
                                str[confname].swap(&block);
                                strname[confname] = outfile_name_i_j;
                            }
                        }
                    }
                }
                for (auto &p : str) {
                    st->analyze(strname[p.first], &p.second);
                }
            } else {
                std::string buffer;
                for (uint32_t i = start_read_num; i < start_read_num + num_reads_block; i++) buffer += str_array[i];
                qualityBuffer = gabac::DataBlock(&buffer);
                st->analyze(outfile_name, &qualityBuffer);
            }
            st->reloadConfigSet();
        }

        //
        // According to the execution profile, this is the 2nd hottest
        // parallel region in genie when compressing a normal fastq
        // file (not an analysis run).
        //
        // It is important that you specify a chunk size of 1 (which
        // becomes the default when schedule=dynamic).  Without it,
        // libgomp effectively serializes a loop with ordered sections.
        //
        // Right now, on a 4 thread run, the 3 worker threads spend
        // ~1/3 of their time idling, waiting on the ordered section.
        // That figure will get worse as we increase the #threads,
        // but better with larger problem sizes.
        //
        // There might be a better way to parallelize the loop.
        //
        uint64_t id_size = 0;
        uint64_t qual_size = 0;

#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(num_thr) schedule(dynamic) reduction(+:id_size) reduction(+:qual_size)
#else
        (void)num_thr;  // Suppress unused parameter warning
#endif
        for (uint64_t block_num = 0; block_num < blocks; ++block_num) {
            uint64_t block_num_offset = start_read_bin / num_reads_per_block;
            uint64_t start_read_num = block_num * num_reads_per_block;
            uint64_t end_read_num = (block_num + 1) * num_reads_per_block;
            if (end_read_num >= num_reads_bin) {
                end_read_num = num_reads_bin;
            }
            uint32_t num_reads_block = (uint32_t)(end_read_num - start_read_num);
            std::string name = file_name.substr(file_name.find_last_of('/') + 1) + "." + std::to_string(block_num_offset + block_num);
            auto raw_data = generate_empty_raw_data();
            std::vector<std::vector<std::vector<gabac::DataBlock>>> generated_streams =
                create_default_streams();
            if (mode == "id") {
                generate_read_id_tokens(str_array + start_read_num, num_reads_block, raw_data[15]);
                for (int i = 0; i < 128; i++) {
                    for (int j = 0; j < 6; j++) {
                        if (raw_data[15][6 * i + j].size() == 0) {
                            continue;
                        }
                        const gabac::EncodingConfiguration *config;
                        if ((st == NULL) || ((config = st->getConfig(
                          name.substr(name.find_last_of('/') + 1) + "."
                          + std::to_string(i) + "." + std::to_string(j)))
                          == NULL)) {
                            config = &configs[15][0];
                        }
                        // FIXME - &configs[15][0]is loop invariant,
                        // but getConfig(name) is not.  suspicious.
                        gabac_compress(*config, &raw_data[15][6 * i + j],
                                       &generated_streams[15][6 * i + j]);
                    }
                }
                std::string file_to_save_streams = id_desc_prefix + std::to_string(block_num_offset + block_num);
                id_size += write_streams_to_file(generated_streams, file_to_save_streams, id_descriptors);
            } else /* mode == "quality" */ {
                for (uint32_t i = start_read_num; i < start_read_num + num_reads_block; i++)
                    for (size_t pos_in_read = 0; pos_in_read < str_array[i].size(); pos_in_read++)
                        raw_data[14][2].push_back((uint8_t)str_array[i][pos_in_read] - 33);  // quality
                for (size_t subseq = 0; subseq < 3; subseq++) {
                    if (raw_data[14][subseq].size() == 0) {
                        continue;
                    }
                    const gabac::EncodingConfiguration *config;
                    if ((st == NULL) || ((config = st->getConfig(name)) == NULL)) {
                        config = &configs[14][subseq];
                    }
                    gabac_compress(*config, &raw_data[14][subseq],
                                   &generated_streams[14][subseq]);
                }
                std::string file_to_save_streams = quality_desc_prefix + std::to_string(block_num_offset + block_num);
                qual_size += write_streams_to_file(generated_streams, file_to_save_streams, quality_descriptors);
            }
        }  // omp parallel

        if (stats->enabled) {
            stats->cmprs_id_sz += id_size;
            stats->cmprs_qual_sz += qual_size;
            stats->cmprs_total_sz += id_size + qual_size;
        }
    }
}

void generate_read_id_tokens(std::string *id_array, const uint32_t &num_ids, std::vector<int64_t> tokens[128][6]) {
    char prev_ID[MAX_NUM_TOKENS_ID] = {0};
    uint32_t prev_tokens_ptr[MAX_NUM_TOKENS_ID] = {0};
    for (uint32_t id_num = 0; id_num < num_ids; id_num++) {
        tokens[0][0].push_back(1);  // DIFF
        if (id_num == 0)
            tokens[0][1].push_back(0);  // DIFF 0 for first id
        else
            tokens[0][1].push_back(1);  // DIFF 1 for rest of ids
        generate_id_tokens(prev_ID, prev_tokens_ptr, id_array[id_num], tokens);
    }
}

void generate_read_id_tokens(std::string *id_array, const uint32_t &num_ids, std::vector<gabac::DataBlock> &tokens) {
    char prev_ID[MAX_NUM_TOKENS_ID] = {0};
    uint32_t prev_tokens_ptr[MAX_NUM_TOKENS_ID] = {0};
    for (uint32_t id_num = 0; id_num < num_ids; id_num++) {
        tokens[0 * 6 + 0].push_back(1);  // DIFF
        if (id_num == 0) {
            big_endian_push_uint32(0, &tokens[0 * 6 + 1]);  // DIFF 0 for first id
        } else {
            big_endian_push_uint32(1, &tokens[0 * 6 + 1]);  // DIFF 1 for rest of ids
        }
        generate_id_tokens(prev_ID, prev_tokens_ptr, id_array[id_num], tokens);
    }
}

}  // namespace spring
