/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit.h>
#include <genie/core/read-encoder.h>
#include <genie/core/record/chunk.h>
#include <genie/core/record/segment.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "reorder-compress-quality-id.h"
#include "util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

// ---------------------------------------------------------------------------------------------------------------------

void reorder_compress_quality_id(const std::string &temp_dir, const compression_params &cp,
                                 genie::core::ReadEncoder::QvSelector *qv_coder,
                                 genie::core::ReadEncoder::NameSelector *name_coder,
                                 genie::core::ReadEncoder::EntropySelector *entropy,
                                 std::vector<core::parameter::ParameterSet> &params, core::stats::PerfStats &stats) {
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
                             str_array_size, order_array, "quality", qv_coder, name_coder, entropy, params, stats);
            remove(file_quality[0].c_str());
        }
        if (preserve_id) {
            std::cout << "Compressing ids\n";
            uint32_t num_reads_per_file = numreads;
            reorder_compress(file_id, temp_dir, num_reads_per_file, num_thr, num_reads_per_block, str_array,
                             str_array_size, order_array, "id", qv_coder, name_coder, entropy, params, stats);
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
            reorder_compress_quality_pe(file_quality, outfile_quality, temp_dir, quality_array, quality_array_size,
                                        order_array, block_start, block_end, cp, qv_coder, entropy, params, stats);
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
            reorder_compress_id_pe(id_array, temp_dir, file_order_id, block_start, block_end, file_id, cp, name_coder,
                                   entropy, params, stats);
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

// ---------------------------------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------------------------------

void generate_order(const std::string &file_order, uint32_t *order_array, const uint32_t &numreads) {
    std::ifstream fin_order(file_order, std::ios::binary);
    uint32_t order;
    for (uint32_t i = 0; i < numreads; i++) {
        fin_order.read((char *)&order, sizeof(uint32_t));
        order_array[order] = i;
    }
    fin_order.close();
}

// ---------------------------------------------------------------------------------------------------------------------

void reorder_compress_id_pe(std::string *id_array, const std::string &temp_dir, const std::string &file_order_id,
                            const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                            const std::string &file_name, const compression_params &cp,
                            genie::core::ReadEncoder::NameSelector *name_coder,
                            genie::core::ReadEncoder::EntropySelector *entropy,
                            std::vector<core::parameter::ParameterSet> &params, core::stats::PerfStats &stats) {
    const std::string id_desc_prefix = temp_dir + "/id_streams.";
    (void)cp;

    std::vector<core::stats::PerfStats> stat_vec(block_start.size());

#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(cp.num_thr) schedule(dynamic)
#endif
    for (int64_t block_num = 0; block_num < static_cast<int64_t>(block_start.size()); block_num++) {
        std::ifstream f_order_id(file_order_id + "." + std::to_string(block_num), std::ios::binary);
        std::string *id_array_block = new std::string[block_end[block_num] - block_start[block_num]];
        uint32_t index;
        for (uint32_t j = block_start[block_num]; j < block_end[block_num]; j++) {
            f_order_id.read((char *)&index, sizeof(uint32_t));
            id_array_block[j - block_start[block_num]] = id_array[index];
        }
        genie::core::record::Chunk chunk;
        for (size_t i = 0; i < block_end[block_num] - block_start[block_num]; ++i) {
            chunk.getData().emplace_back(1, core::record::ClassType::CLASS_U, std::move(id_array_block[i]), "", 0);
            chunk.getData().back().addSegment(core::record::Segment("N"));
        }
        auto raw_desc = name_coder->process(chunk);
        stat_vec[block_num].add(std::get<1>(raw_desc));
        chunk.getData().clear();
        auto encoded = entropy->process(std::get<0>(raw_desc));
        stat_vec[block_num].add(std::get<2>(encoded));
        std::string name = file_name + "." + std::to_string(block_num);
        params[block_num].setDescriptor(core::GenDesc::RNAME, std::move(std::get<0>(encoded)));
        std::string file_to_save_streams = id_desc_prefix + std::to_string(block_num);
        std::ofstream outfile(file_to_save_streams, std::ios::binary);
        util::BitWriter bw(&outfile);
        std::get<1>(encoded).write(bw);

        f_order_id.close();
        delete[] id_array_block;
    }

    for (const auto &s : stat_vec) {
        stats.add(s);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void reorder_compress_quality_pe(std::string file_quality[2], const std::string &outfile_quality,
                                 const std::string &temp_dir, std::string *quality_array,
                                 const uint64_t &quality_array_size, uint32_t *order_array,
                                 const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                                 const compression_params &cp, genie::core::ReadEncoder::QvSelector *qv_coder,
                                 genie::core::ReadEncoder::EntropySelector *entropy,
                                 std::vector<core::parameter::ParameterSet> &params, core::stats::PerfStats &stats) {
    const std::string quality_desc_prefix = temp_dir + "/quality_streams.";
    uint32_t start_block_num = 0;
    uint32_t end_block_num = 0;
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

        std::vector<core::stats::PerfStats> stat_vec(end_block_num - start_block_num);

#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(cp.num_thr) schedule(dynamic)
#endif

        for (int64_t block_num = start_block_num; block_num < end_block_num; block_num++) {
            genie::core::record::Chunk chunk;
            for (size_t i = block_start[block_num]; i < block_end[block_num]; i++) {
                chunk.getData().emplace_back(2, core::record::ClassType::CLASS_U, "", "", 0);
                core::record::Segment s(std::string(quality_array[i - block_start[start_block_num]].size(), 'N'));
                s.addQualities(std::move(quality_array[i - block_start[start_block_num]]));
                chunk.getData().back().addSegment(std::move(s));
            }

            std::string name = outfile_quality + "." + std::to_string(block_num);
            auto raw_desc = qv_coder->process(chunk);
            params[block_num].setQVDepth(std::get<1>(raw_desc).isEmpty() ? 0 : 1);
            stat_vec[block_num - start_block_num].add(std::get<2>(raw_desc));
            chunk.getData().clear();
            auto encoded = entropy->process(std::get<1>(raw_desc));
            stat_vec[block_num - start_block_num].add(std::get<2>(encoded));
            params[block_num].addClass(core::record::ClassType::CLASS_U, std::move(std::get<0>(raw_desc)));
            params[block_num].setDescriptor(core::GenDesc::QV, std::move(std::get<0>(encoded)));
            std::string file_to_save_streams = quality_desc_prefix + std::to_string(block_num);
            std::ofstream out(file_to_save_streams,std::ios::binary);
            util::BitWriter bw(&out);
            std::get<1>(encoded).write(bw);
        }
        start_block_num = end_block_num;

        for (const auto &s : stat_vec) {
            stats.add(s);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void reorder_compress(const std::string &file_name, const std::string &temp_dir, const uint32_t &num_reads_per_file,
                      const int &num_thr, const uint32_t &num_reads_per_block, std::string *str_array,
                      const uint32_t &str_array_size, uint32_t *order_array, const std::string &mode,
                      genie::core::ReadEncoder::QvSelector *qv_coder,
                      genie::core::ReadEncoder::NameSelector *name_coder,
                      genie::core::ReadEncoder::EntropySelector *entropy,
                      std::vector<core::parameter::ParameterSet> &params, core::stats::PerfStats &stats) {
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

        std::vector<core::stats::PerfStats> stat_vec(blocks);

#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(num_thr) schedule(dynamic)
#else
        (void)num_thr;  // Suppress unused parameter warning
#endif
        for (int64_t block_num = 0; block_num < static_cast<int64_t>(blocks); ++block_num) {
            uint64_t block_num_offset = start_read_bin / num_reads_per_block;
            uint64_t start_read_num = block_num * num_reads_per_block;
            uint64_t end_read_num = (block_num + 1) * num_reads_per_block;
            if (end_read_num >= num_reads_bin) {
                end_read_num = num_reads_bin;
            }
            uint32_t num_reads_block = (uint32_t)(end_read_num - start_read_num);
            std::string name =
                file_name.substr(file_name.find_last_of('/') + 1) + "." + std::to_string(block_num_offset + block_num);
            if (mode == "id") {
                genie::core::record::Chunk chunk;
                for (size_t i = 0; i < num_reads_block; i++) {
                    chunk.getData().emplace_back(1, core::record::ClassType::CLASS_U,
                                                 std::move(str_array[start_read_num + i]), "", 0);
                    core::record::Segment s("N");
                    chunk.getData().back().addSegment(std::move(s));
                }

                auto name_raw = name_coder->process(chunk);
                stat_vec[block_num].add(std::get<1>(name_raw));
                auto encoded = entropy->process(std::get<0>(name_raw));
                stat_vec[block_num].add(std::get<2>(encoded));
                params[block_num_offset + block_num].setDescriptor(core::GenDesc::RNAME,
                                                                   std::move(std::get<0>(encoded)));
                std::string file_to_save_streams = id_desc_prefix + std::to_string(block_num_offset + block_num);
                std::ofstream out(file_to_save_streams,std::ios::binary);
                util::BitWriter bw(&out);
                std::get<1>(encoded).write(bw);
            } else /* mode == "quality" */ {
                genie::core::record::Chunk chunk;
                for (uint32_t i = start_read_num; i < start_read_num + num_reads_block; i++) {
                    chunk.getData().emplace_back(1, core::record::ClassType::CLASS_U, "", "", 0);
                    core::record::Segment s(std::string(str_array[i].size(), 'N'));
                    s.addQualities(std::move(str_array[i]));
                    chunk.getData().back().addSegment(std::move(s));
                }
                auto qv_str = qv_coder->process(chunk);
                stat_vec[block_num].add(std::get<2>(qv_str));
                params[block_num_offset + block_num].setQVDepth(std::get<1>(qv_str).isEmpty() ? 0 : 1);
                auto encoded = entropy->process(std::get<1>(qv_str));
                stat_vec[block_num].add(std::get<2>(encoded));
                params[block_num_offset + block_num].addClass(core::record::ClassType::CLASS_U,
                                                              std::move(std::get<0>(qv_str)));
                params[block_num_offset + block_num].setDescriptor(core::GenDesc::QV, std::move(std::get<0>(encoded)));
                std::string file_to_save_streams = quality_desc_prefix + std::to_string(block_num_offset + block_num);
                std::ofstream out(file_to_save_streams,std::ios::binary);
                util::BitWriter bw(&out);
                std::get<1>(encoded).write(bw);
            }
        }  // omp parallel

        for (const auto &s : stat_vec) {
            stats.add(s);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
