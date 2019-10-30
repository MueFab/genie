#ifdef GENIE_USE_OPENMP

#include <omp.h>

#endif

#include <array>
#include <cmath>  // abs
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "generate-read-streams.h"
#include "spring-gabac.h"
#include "util.h"
#include <format/part2/clutter.h>

namespace spring {

void generate_read_streams(const std::string &temp_dir, const compression_params &cp, const std::vector<std::vector<gabac::EncodingConfiguration>>& configs) {
    if (!cp.paired_end)
        generate_read_streams_se(temp_dir, cp, configs);
    else
        generate_read_streams_pe(temp_dir, cp, configs);
}

void compress_read_subseqs(std::vector<std::vector<gabac::DataBlock>> &raw_data,
                           std::vector<std::vector<std::vector<gabac::DataBlock>>> &generated_streams,
                           const std::vector<std::vector<gabac::EncodingConfiguration>> &configs) {
    for (size_t descriptor = 0; descriptor < 13; ++descriptor) {
        for (size_t subdescriptor = 0; subdescriptor < raw_data[descriptor].size(); ++subdescriptor) {
            gabac_compress(configs[descriptor][subdescriptor], &raw_data[descriptor][subdescriptor],
                           &generated_streams[descriptor][subdescriptor]);
        }
    }
}

struct se_data {
    compression_params cp;
    std::vector<bool> flag_arr;
    std::vector<uint64_t> pos_arr;
    std::vector<uint16_t> read_length_arr;
    std::vector<char> noise_arr;
    std::vector<uint16_t> noisepos_arr;
    std::vector<uint64_t> pos_in_noise_arr;
    std::vector<uint16_t> noise_len_arr;
    std::vector<char> unaligned_arr;
    std::string seq;
    std::vector<char> RC_arr;
    std::vector<uint32_t> order_arr;
};

void generate_subseqs(const se_data &data, uint64_t block_num, std::vector<std::vector<gabac::DataBlock>> &raw_data) {
    // char_to_int
    int64_t char_to_int[128];
    char_to_int[(uint8_t)'A'] = 0;
    char_to_int[(uint8_t)'C'] = 1;
    char_to_int[(uint8_t)'G'] = 2;
    char_to_int[(uint8_t)'T'] = 3;
    char_to_int[(uint8_t)'N'] = 4;

    int64_t rc_to_int[128];
    rc_to_int[(uint8_t)'d'] = 0;
    rc_to_int[(uint8_t)'r'] = 1;

    uint64_t start_read_num = block_num * data.cp.num_reads_per_block;
    uint64_t end_read_num = (block_num + 1) * data.cp.num_reads_per_block;

    if (end_read_num >= data.cp.num_reads) {
        end_read_num = data.cp.num_reads;
    }

    // first find the seq
    uint64_t seq_start, seq_end;
    if (data.flag_arr[start_read_num] == false)
        seq_start = seq_end = 0;  // all reads unaligned
    else {
        seq_end = seq_start = data.pos_arr[start_read_num];
        // find last read in AU that's aligned
        uint64_t i = start_read_num;
        for (; i < end_read_num; i++) {
            if (data.flag_arr[i] == false) break;
            seq_end = std::max(seq_end, data.pos_arr[i] + data.read_length_arr[i]);
        }
    }
    if (seq_start != seq_end) {
        // not all unaligned
        raw_data[7][0].push_back(seq_end - seq_start - 1);  // rlen
        raw_data[12][0].push_back(5);                       // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
            raw_data[6][0].push_back(char_to_int[(uint8_t)data.seq[i]]);  // ureads
    }
    uint64_t prevpos = 0, diffpos;
    // Write streams
    for (uint64_t i = start_read_num; i < end_read_num; i++) {
        if (data.flag_arr[i] == true) {
            raw_data[7][0].push_back(data.read_length_arr[i] - 1);         // rlen
            raw_data[1][0].push_back(rc_to_int[(uint8_t)data.RC_arr[i]]);  // rcomp
            if (i == start_read_num) {
                // Note: In order non-preserving mode, if the first read of
                // the block is a singleton, then the rest are too.
                raw_data[0][0].push_back(0);  // pos
                prevpos = data.pos_arr[i];
            } else {
                diffpos = data.pos_arr[i] - prevpos;
                raw_data[0][0].push_back(diffpos);  // pos
                prevpos = data.pos_arr[i];
            }
            if (data.noise_len_arr[i] == 0)
                raw_data[12][0].push_back(1);  // rtype = P
            else {
                raw_data[12][0].push_back(3);  // rtype = M
                for (uint16_t j = 0; j < data.noise_len_arr[i]; j++) {
                    raw_data[3][0].push_back(0);  // mmpos
                    if (j == 0)
                        raw_data[3][1].push_back(data.noisepos_arr[data.pos_in_noise_arr[i] + j]);
                    else
                        raw_data[3][1].push_back(data.noisepos_arr[data.pos_in_noise_arr[i] + j] -
                                                 1);  // decoder adds +1
                    raw_data[4][0].push_back(0);      // mmtype = Substitution
                    raw_data[4][1].push_back(char_to_int[(uint8_t)data.noise_arr[data.pos_in_noise_arr[i] + j]]);
                }
                raw_data[3][0].push_back(1);
            }
        } else {
            raw_data[12][0].push_back(5);                           // rtype
            raw_data[7][0].push_back(data.read_length_arr[i] - 1);  // rlen
            for (uint64_t j = 0; j < data.read_length_arr[i]; j++) {
                raw_data[6][0].push_back(char_to_int[(uint8_t)data.unaligned_arr[data.pos_arr[i] + j]]);  // ureads
            }
            raw_data[0][0].push_back(seq_end - prevpos);            // pos
            raw_data[1][0].push_back(0);                            // rcomp
            raw_data[7][0].push_back(data.read_length_arr[i] - 1);  // rlen
            raw_data[12][0].push_back(1);                           // rtype = P
            prevpos = seq_end;
            seq_end = prevpos + data.read_length_arr[i];
        }
    }
}

void generate_and_compress_se(const std::string &temp_dir, const se_data &data, const std::vector<std::vector<gabac::EncodingConfiguration>> &configs) {
    // Now generate new streams and compress blocks in parallel
    // this is actually number of read pairs per block for PE
    uint64_t blocks = uint64_t(std::ceil(float(data.cp.num_reads) / data.cp.num_reads_per_block));
    std::vector<uint32_t> num_reads_per_block(blocks);
#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(data.cp.num_thr) schedule(dynamic)
#endif
    for (uint64_t block_num = 0; block_num < blocks; block_num++) {
        auto raw_data = generate_empty_raw_data();

        generate_subseqs(data, block_num, raw_data);
        num_reads_per_block[block_num] = raw_data[1][0].size(); // rcomp

        std::vector<std::vector<std::vector<gabac::DataBlock>>> generated_streams = create_default_streams();
        compress_read_subseqs(raw_data, generated_streams, configs);

        std::string file_to_save_streams = temp_dir + "/read_streams." + std::to_string(block_num);
        write_streams_to_file(generated_streams, file_to_save_streams, read_descriptors);

    }  // end omp parallel

    // write num blocks, reads per block to a file
    const std::string block_info_file = temp_dir + "/block_info.bin";
    std::ofstream f_block_info(block_info_file, std::ios::binary);
    uint32_t num_blocks = (uint32_t)blocks;
    f_block_info.write((char*)&num_blocks, sizeof(uint32_t));
    f_block_info.write((char*)&num_reads_per_block[0],num_blocks*sizeof(uint32_t));

    return;
}

void loadSE_Data(const compression_params &cp, const std::string &temp_dir, se_data *data) {
    const std::string file_seq = temp_dir + "/read_seq.txt";
    const std::string file_pos = temp_dir + "/read_pos.bin";
    const std::string file_RC = temp_dir + "/read_rev.txt";
    const std::string file_readlength = temp_dir + "/read_lengths.bin";
    const std::string file_unaligned = temp_dir + "/read_unaligned.txt";
    const std::string file_noise = temp_dir + "/read_noise.txt";
    const std::string file_noisepos = temp_dir + "/read_noisepos.bin";
    const std::string file_order = temp_dir + "/read_order.bin";

    data->cp = cp;
    // load some params
    uint32_t num_reads = cp.num_reads, num_reads_aligned = 0, num_reads_unaligned;
    data->RC_arr = std::vector<char>(cp.num_reads);
    data->read_length_arr = std::vector<uint16_t>(cp.num_reads);
    data->flag_arr = std::vector<bool>(cp.num_reads);
    data->pos_in_noise_arr = std::vector<uint64_t>(cp.num_reads);
    data->pos_arr = std::vector<uint64_t>(cp.num_reads);
    data->noise_len_arr = std::vector<uint16_t>(cp.num_reads);

    // read streams for aligned reads
    std::ifstream f_seq(file_seq);
    f_seq.seekg(0, f_seq.end);
    uint64_t seq_len = f_seq.tellg();
    data->seq.resize(seq_len);
    f_seq.seekg(0);
    f_seq.read(&data->seq[0], seq_len);
    f_seq.close();
    std::ifstream f_order;
    std::ifstream f_RC(file_RC);
    std::ifstream f_readlength(file_readlength, std::ios::binary);
    std::ifstream f_noise(file_noise);
    std::ifstream f_noisepos(file_noisepos, std::ios::binary);
    std::ifstream f_pos(file_pos, std::ios::binary);
    f_noisepos.seekg(0, f_noisepos.end);
    uint64_t noise_array_size = f_noisepos.tellg() / 2;
    f_noisepos.seekg(0, f_noisepos.beg);
    // divide by 2 because we have 2 bytes per noise
    data->noise_arr = std::vector<char>(noise_array_size);
    data->noisepos_arr = std::vector<uint16_t>(noise_array_size);
    char rc, noise_char;
    uint32_t order = 0;
    uint64_t current_pos_noise_arr = 0;
    uint64_t current_pos_noisepos_arr = 0;
    uint64_t pos;
    uint16_t num_noise_in_curr_read;
    uint16_t read_length, noisepos;

    while (f_RC.get(rc)) {
        f_readlength.read((char *)&read_length, sizeof(uint16_t));
        f_pos.read((char *)&pos, sizeof(uint64_t));
        data->RC_arr[order] = rc;
        data->read_length_arr[order] = read_length;
        data->flag_arr[order] = true;  // aligned
        data->pos_arr[order] = pos;
        data->pos_in_noise_arr[order] = current_pos_noise_arr;
        num_noise_in_curr_read = 0;
        f_noise.get(noise_char);
        while (noise_char != '\n') {
            data->noise_arr[current_pos_noise_arr++] = noise_char;
            num_noise_in_curr_read++;
            f_noise.get(noise_char);
        }
        for (uint16_t i = 0; i < num_noise_in_curr_read; i++) {
            f_noisepos.read((char *)&noisepos, sizeof(uint16_t));
            data->noisepos_arr[current_pos_noisepos_arr] = noisepos;
            current_pos_noisepos_arr++;
        }
        data->noise_len_arr[order] = num_noise_in_curr_read;
        num_reads_aligned++;
        order++;
    }
    f_noise.close();
    f_noisepos.close();
    f_RC.close();
    f_pos.close();

    // Now start with unaligned reads
    num_reads_unaligned = num_reads - num_reads_aligned;
    std::ifstream f_unaligned(file_unaligned);
    f_unaligned.seekg(0, f_unaligned.end);
    uint64_t unaligned_array_size = f_unaligned.tellg();
    f_unaligned.seekg(0, f_unaligned.beg);
    data->unaligned_arr = std::vector<char>(unaligned_array_size);
    f_unaligned.read(data->unaligned_arr.data(), unaligned_array_size);
    f_unaligned.close();
    uint64_t current_pos_in_unaligned_arr = 0;
    for (uint32_t i = 0; i < num_reads_unaligned; i++) {
        f_readlength.read((char *)&read_length, sizeof(uint16_t));
        data->read_length_arr[order] = read_length;
        data->pos_arr[order] = current_pos_in_unaligned_arr;
        current_pos_in_unaligned_arr += read_length;
        data->flag_arr[order] = false;  // unaligned
        order++;
    }
    f_readlength.close();

    // delete old streams
    remove(file_noise.c_str());
    remove(file_noisepos.c_str());
    remove(file_RC.c_str());
    remove(file_readlength.c_str());
    remove(file_unaligned.c_str());
    remove(file_pos.c_str());
    remove(file_seq.c_str());
}

void generate_read_streams_se(const std::string &temp_dir,
                              const compression_params &cp,
                              const std::vector<std::vector<gabac::EncodingConfiguration>> &configs) {
    se_data data;
    loadSE_Data(cp, temp_dir, &data);

    generate_and_compress_se(temp_dir, data, configs);

    return;
}

void loadPE_Data(const compression_params &cp, const std::string &temp_dir, se_data *data) {
    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> descriptorFilesPerAU;

    const std::string file_seq = temp_dir + "/read_seq.txt";
    const std::string file_pos = temp_dir + "/read_pos.bin";
    const std::string file_RC = temp_dir + "/read_rev.txt";
    const std::string file_readlength = temp_dir + "/read_lengths.bin";
    const std::string file_unaligned = temp_dir + "/read_unaligned.txt";
    const std::string file_noise = temp_dir + "/read_noise.txt";
    const std::string file_noisepos = temp_dir + "/read_noisepos.bin";
    const std::string file_order = temp_dir + "/read_order.bin";

    data->cp = cp;

    // load some params
    uint32_t num_reads_aligned = 0, num_reads_unaligned;
    data->RC_arr = std::vector<char>(cp.num_reads);
    data->read_length_arr = std::vector<uint16_t>(cp.num_reads);
    data->flag_arr = std::vector<bool>(cp.num_reads);
    data->pos_in_noise_arr = std::vector<uint64_t>(cp.num_reads);
    data->pos_arr = std::vector<uint64_t>(cp.num_reads);
    data->noise_len_arr = std::vector<uint16_t>(cp.num_reads);

    data->order_arr = std::vector<uint32_t>(cp.num_reads);
    // PE step 1: read all streams indexed by original position in FASTQ, also
    // read order array

    // read streams for aligned reads
    std::ifstream f_seq(file_seq);
    f_seq.seekg(0, f_seq.end);
    uint64_t seq_len = f_seq.tellg();
    data->seq.resize(seq_len);
    f_seq.seekg(0);
    f_seq.read(&data->seq[0], seq_len);
    f_seq.close();
    std::ifstream f_order;
    f_order.open(file_order, std::ios::binary);
    std::ifstream f_RC(file_RC);
    std::ifstream f_readlength(file_readlength, std::ios::binary);
    std::ifstream f_noise(file_noise);
    std::ifstream f_noisepos(file_noisepos, std::ios::binary);
    std::ifstream f_pos(file_pos, std::ios::binary);
    f_noisepos.seekg(0, f_noisepos.end);
    uint64_t noise_array_size = f_noisepos.tellg() / 2;
    f_noisepos.seekg(0, f_noisepos.beg);
    // divide by 2 because we have 2 bytes per noise
    data->noise_arr = std::vector<char>(noise_array_size);
    data->noisepos_arr = std::vector<uint16_t>(noise_array_size);
    char rc, noise_char;
    uint32_t order = 0;
    uint64_t current_pos_noise_arr = 0;
    uint64_t current_pos_noisepos_arr = 0;
    uint64_t pos;
    uint16_t num_noise_in_curr_read;
    uint16_t read_length, noisepos;
    uint32_t ind = 0;
    while (f_RC.get(rc)) {
        f_order.read((char *)&order, sizeof(uint32_t));
        data->order_arr[ind++] = order;
        f_readlength.read((char *)&read_length, sizeof(uint16_t));
        f_pos.read((char *)&pos, sizeof(uint64_t));
        data->RC_arr[order] = rc;
        data->read_length_arr[order] = read_length;
        data->flag_arr[order] = true;  // aligned
        data->pos_arr[order] = pos;
        data->pos_in_noise_arr[order] = current_pos_noise_arr;
        num_noise_in_curr_read = 0;
        f_noise.get(noise_char);
        while (noise_char != '\n') {
            data->noise_arr[current_pos_noise_arr++] = noise_char;
            num_noise_in_curr_read++;
            f_noise.get(noise_char);
        }
        for (uint16_t i = 0; i < num_noise_in_curr_read; i++) {
            f_noisepos.read((char *)&noisepos, sizeof(uint16_t));
            data->noisepos_arr[current_pos_noisepos_arr] = noisepos;
            current_pos_noisepos_arr++;
        }
        data->noise_len_arr[order] = num_noise_in_curr_read;
        num_reads_aligned++;
    }
    f_noise.close();
    f_noisepos.close();
    f_RC.close();
    f_pos.close();

    // Now start with unaligned reads
    num_reads_unaligned = cp.num_reads - num_reads_aligned;
    std::ifstream f_unaligned(file_unaligned);
    f_unaligned.seekg(0, f_unaligned.end);
    uint64_t unaligned_array_size = f_unaligned.tellg();
    f_unaligned.seekg(0, f_unaligned.beg);
    data->unaligned_arr = std::vector<char>(unaligned_array_size);
    f_unaligned.read(&data->unaligned_arr[0], unaligned_array_size);
    f_unaligned.close();
    uint64_t current_pos_in_unaligned_arr = 0;
    for (uint32_t i = 0; i < num_reads_unaligned; i++) {
        f_order.read((char *)&order, sizeof(uint32_t));
        data->order_arr[ind++] = order;
        f_readlength.read((char *)&read_length, sizeof(uint16_t));
        data->read_length_arr[order] = read_length;
        data->pos_arr[order] = current_pos_in_unaligned_arr;
        current_pos_in_unaligned_arr += read_length;
        data->flag_arr[order] = false;  // unaligned
    }
    f_order.close();
    f_readlength.close();

    // delete old streams
    remove(file_noise.c_str());
    remove(file_noisepos.c_str());
    remove(file_RC.c_str());
    remove(file_readlength.c_str());
    remove(file_unaligned.c_str());
    remove(file_pos.c_str());
    remove(file_seq.c_str());
}

struct pe_block_data {
    std::vector<uint32_t> block_start;
    std::vector<uint32_t> block_end;  // block start and end positions wrt
    std::vector<uint32_t> block_num;
    std::vector<uint32_t> genomic_record_index;
    std::vector<uint32_t> read_index_genomic_record;
    std::vector<uint32_t> block_seq_start;
    std::vector<uint32_t> block_seq_end;
};

void generateBlocksPE(const se_data &data, pe_block_data *bdata) {
    bdata->block_num = std::vector<uint32_t>(data.cp.num_reads);

    bdata->genomic_record_index = std::vector<uint32_t>(data.cp.num_reads);

    // PE step 2: decide on the blocks for the reads
    uint32_t num_records_current_block = 0;
    uint32_t current_block_num = 0;
    uint32_t current_block_seq_end = 0;
    bdata->block_start.push_back(0);

    std::vector<bool> already_seen(data.cp.num_reads, false);
    std::vector<uint32_t> to_push_at_end_of_block;

    for (uint32_t i = 0; i < data.cp.num_reads; i++) {
        uint32_t current = data.order_arr[i];
        if (num_records_current_block == 0) {
            // first read of block
            if (!data.flag_arr[current])
                bdata->block_seq_start.push_back(0);
            else
                bdata->block_seq_start.push_back(data.pos_arr[current]);
        }
        if (!already_seen[current]) {
            // current is already seen when current is unaligned and its pair has
            // already appeared before - in such cases we don't need to handle it now.
            already_seen[current] = true;
            uint32_t pair = (current < data.cp.num_reads / 2) ? (current + data.cp.num_reads / 2)
                                                              : (current - data.cp.num_reads / 2);
            if (already_seen[pair]) {
                if (bdata->block_num[pair] == current_block_num && (data.pos_arr[current] >= data.pos_arr[pair]) &&
                    (data.pos_arr[current] - data.pos_arr[pair] < 32768)) {
                    // put in same record
                    bdata->block_num[current] = current_block_num;
                    bdata->genomic_record_index[current] = bdata->genomic_record_index[pair];
                } else {
                    // new genomic record
                    bdata->block_num[current] = current_block_num;
                    bdata->genomic_record_index[current] = num_records_current_block++;
                    bdata->read_index_genomic_record.push_back(current);
                }
            } else {
                // add new genomic record
                bdata->block_num[current] = current_block_num;
                bdata->genomic_record_index[current] = num_records_current_block++;

                if (!data.flag_arr[pair]) {
                    already_seen[pair] = true;
                    if (!data.flag_arr[current]) {
                        // both unaligned - put in same record
                        bdata->block_num[pair] = current_block_num;
                        bdata->genomic_record_index[pair] = num_records_current_block - 1;
                        bdata->read_index_genomic_record.push_back(
                            std::min(current, pair));  // always put read 1 first in this case
                    } else {
                        // pair is unaligned, put in same block at end (not in same record)
                        // for now, put in to_push_at_end_of_block vector (processed at end
                        // of block)
                        bdata->read_index_genomic_record.push_back(current);
                        to_push_at_end_of_block.push_back(pair);
                    }
                } else {
                    bdata->read_index_genomic_record.push_back(current);
                }
            }
            if (data.flag_arr[current]) {
                // update current_block_seq_end
                if (current_block_seq_end < data.pos_arr[current] + data.read_length_arr[current])
                    current_block_seq_end = data.pos_arr[current] + data.read_length_arr[current];
            }
        }
        if (num_records_current_block == data.cp.num_reads_per_block || i == data.cp.num_reads - 1) {
            // block done
            // put in the reads in to_push_at_end_of_block vector and clear the vector
            for (uint32_t j : to_push_at_end_of_block) {
                bdata->block_num[j] = current_block_num;
                bdata->genomic_record_index[j] = num_records_current_block++;
                bdata->read_index_genomic_record.push_back(j);
            }
            to_push_at_end_of_block.clear();
            bdata->block_end.push_back(bdata->block_start.back() + num_records_current_block);
            bdata->block_seq_end.push_back(current_block_seq_end);

            if (i != data.cp.num_reads - 1) {
                // start new block
                num_records_current_block = 0;
                current_block_num++;
                current_block_seq_end = 0;
                bdata->block_start.push_back(bdata->read_index_genomic_record.size());
            }
        }
    }
    already_seen.clear();
}

void generate_qual_id_pe(const std::string &temp_dir, const pe_block_data &bdata, uint32_t num_reads) {
    // PE step 3: generate index for ids and quality

    const std::string file_order_quality = temp_dir + "/order_quality.bin";
    const std::string file_blocks_quality = temp_dir + "/blocks_quality.bin";
    const std::string file_order_id = temp_dir + "/order_id.bin";
    const std::string file_blocks_id = temp_dir + "/blocks_id.bin";

    // quality:
    std::ofstream f_order_quality(file_order_quality, std::ios::binary);
    // store order (as usual in uint32_t)
    std::ofstream f_blocks_quality(file_blocks_quality, std::ios::binary);
    // store block start and end positions (differs from the block_start and end
    // because here we measure in terms of quality values rather than records
    uint32_t quality_block_pos = 0;
    for (uint32_t i = 0; i < bdata.block_start.size(); i++) {
        f_blocks_quality.write((char *)&quality_block_pos, sizeof(uint32_t));
        for (uint32_t j = bdata.block_start[i]; j < bdata.block_end[i]; j++) {
            uint32_t current = bdata.read_index_genomic_record[j];
            uint32_t pair = (current < num_reads / 2) ? (current + num_reads / 2) : (current - num_reads / 2);
            if ((bdata.block_num[current] == bdata.block_num[pair]) &&
                (bdata.genomic_record_index[pair] == bdata.genomic_record_index[current])) {
                // pair in genomic record
                f_order_quality.write((char *)&current, sizeof(uint32_t));
                quality_block_pos++;
                f_order_quality.write((char *)&pair, sizeof(uint32_t));
                quality_block_pos++;
            } else {
                // only single read in genomic record
                f_order_quality.write((char *)&current, sizeof(uint32_t));
                quality_block_pos++;
            }
        }
        f_blocks_quality.write((char *)&quality_block_pos, sizeof(uint32_t));
    }
    f_order_quality.close();
    f_blocks_quality.close();
    // id:
    std::ofstream f_blocks_id(file_blocks_id, std::ios::binary);
    // store block start and end positions (measured in terms of records since 1
    // record = 1 id)
    for (uint32_t i = 0; i < bdata.block_start.size(); i++) {
        f_blocks_id.write((char *)&bdata.block_start[i], sizeof(uint32_t));
        f_blocks_id.write((char *)&bdata.block_end[i], sizeof(uint32_t));
        std::ofstream f_order_id(file_order_id + "." + std::to_string(i), std::ios::binary);
        // store order
        for (uint32_t j = bdata.block_start[i]; j < bdata.block_end[i]; j++) {
            uint32_t current = bdata.read_index_genomic_record[j];
            uint32_t pair = (current < num_reads / 2) ? (current + num_reads / 2) : (current - num_reads / 2);
            // just write the min of current and pair
            uint32_t min_index = (current > pair) ? pair : current;
            f_order_id.write((char *)&min_index, sizeof(uint32_t));
        }
        f_order_id.close();
    }
    f_blocks_id.close();
}

struct pe_statistics {
    std::vector<uint32_t> count_same_rec;
    std::vector<uint32_t> count_split_same_AU;
    std::vector<uint32_t> count_split_diff_AU;
};

void generate_streams_pe(const se_data &data, const pe_block_data &bdata, uint64_t cur_block_num,
                         std::vector<std::vector<gabac::DataBlock>> &raw_data, pe_statistics *stats) {
#ifdef GENIE_USE_OPENMP
    const unsigned cur_thread_num = omp_get_thread_num();
#else
    const unsigned cur_thread_num = 0;
#endif

    // char_to_int
    int64_t char_to_int[128];
    char_to_int[(uint8_t)'A'] = 0;
    char_to_int[(uint8_t)'C'] = 1;
    char_to_int[(uint8_t)'G'] = 2;
    char_to_int[(uint8_t)'T'] = 3;
    char_to_int[(uint8_t)'N'] = 4;

    int64_t rc_to_int[128];
    rc_to_int[(uint8_t)'d'] = 0;
    rc_to_int[(uint8_t)'r'] = 1;

    // first find the seq
    uint64_t seq_start = bdata.block_seq_start[cur_block_num], seq_end = bdata.block_seq_end[cur_block_num];
    if (seq_start != seq_end) {
        // not all unaligned
        raw_data[7][0].push_back(seq_end - seq_start - 1);  // rlen
        raw_data[12][0].push_back(5);                       // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
            raw_data[6][0].push_back(char_to_int[(uint8_t)data.seq[i]]);  // ureads
    }
    uint64_t prevpos = 0, diffpos;
    // Write streams
    for (uint32_t i = bdata.block_start[cur_block_num]; i < bdata.block_end[cur_block_num]; i++) {
        uint32_t current = bdata.read_index_genomic_record[i];
        uint32_t pair =
            (current < data.cp.num_reads / 2) ? (current + data.cp.num_reads / 2) : (current - data.cp.num_reads / 2);

        if (data.flag_arr[current] == true) {
            if (i == bdata.block_start[cur_block_num]) {
                // Note: In order non-preserving mode, if the first read of
                // the block is a singleton, then the rest are too.
                raw_data[0][0].push_back(0);  // pos
                prevpos = data.pos_arr[current];
            } else {
                diffpos = data.pos_arr[current] - prevpos;
                raw_data[0][0].push_back(diffpos);  // pos
                prevpos = data.pos_arr[current];
            }
        }
        if ((bdata.block_num[current] == bdata.block_num[pair]) &&
            (bdata.genomic_record_index[current] == bdata.genomic_record_index[pair])) {
            // both reads in same record
            if (data.flag_arr[current] == false) {
                // Case 1: both unaligned
                raw_data[12][0].push_back(5);                                                              // rtype
                raw_data[7][0].push_back(data.read_length_arr[current] + data.read_length_arr[pair] - 1);  // rlen
                for (uint64_t j = 0; j < data.read_length_arr[current]; j++) {
                    raw_data[6][0].push_back(
                        char_to_int[(uint8_t)data.unaligned_arr[data.pos_arr[current] + j]]);  // ureads
                }
                for (uint64_t j = 0; j < data.read_length_arr[pair]; j++) {
                    raw_data[6][0].push_back(
                        char_to_int[(uint8_t)data.unaligned_arr[data.pos_arr[pair] + j]]);  // ureads
                }
                raw_data[0][0].push_back(seq_end - prevpos);                  // pos
                raw_data[1][0].push_back(0);                                  // rcomp
                raw_data[1][0].push_back(0);                                  // rcomp
                raw_data[7][0].push_back(data.read_length_arr[current] - 1);  // rlen
                raw_data[7][0].push_back(data.read_length_arr[pair] - 1);     // rlen
                raw_data[12][0].push_back(1);                                 // rtype = P
                raw_data[8][0].push_back(0);                                  // pair decoding case same_rec
                bool read_1_first = true;
                uint16_t delta = data.read_length_arr[current];
                raw_data[8][1].push_back(!(read_1_first) + 2 * delta);  // pair
                prevpos = seq_end;
                seq_end = prevpos + data.read_length_arr[current] + data.read_length_arr[pair];
            } else {
                // Case 2: both aligned
                raw_data[7][0].push_back(data.read_length_arr[current] - 1);         // rlen
                raw_data[7][0].push_back(data.read_length_arr[pair] - 1);            // rlen
                raw_data[1][0].push_back(rc_to_int[(uint8_t)data.RC_arr[current]]);  // rcomp
                raw_data[1][0].push_back(rc_to_int[(uint8_t)data.RC_arr[pair]]);     // rcomp
                if (data.noise_len_arr[current] == 0 && data.noise_len_arr[pair] == 0)
                    raw_data[12][0].push_back(1);  // rtype = P
                else {
                    raw_data[12][0].push_back(3);  // rtype = M
                    for (int k = 0; k < 2; k++) {
                        uint32_t index = k ? pair : current;
                        for (uint16_t j = 0; j < data.noise_len_arr[index]; j++) {
                            raw_data[3][0].push_back(0);  // mmpos
                            if (j == 0)
                                raw_data[3][1].push_back(data.noisepos_arr[data.pos_in_noise_arr[index] + j]);  // mmpos
                            else
                                raw_data[3][1].push_back(data.noisepos_arr[data.pos_in_noise_arr[index] + j] -
                                                         1);  // mmpos
                            raw_data[4][0].push_back(0);      // mmtype = Substitution
                            raw_data[4][1].push_back(
                                char_to_int[(uint8_t)data.noise_arr[data.pos_in_noise_arr[index] + j]]);
                        }
                        raw_data[3][0].push_back(1);  // mmpos
                    }
                }
                bool read_1_first = (current < pair);
                uint16_t delta = data.pos_arr[pair] - data.pos_arr[current];
                raw_data[8][0].push_back(0);                            // pair decoding case same_rec
                raw_data[8][1].push_back(!(read_1_first) + 2 * delta);  // pair
                stats->count_same_rec[cur_thread_num]++;
            }
        } else {
            // only one read in genomic record
            if (data.flag_arr[current] == true) {
                raw_data[7][0].push_back(data.read_length_arr[current] - 1);         // rlen
                raw_data[1][0].push_back(rc_to_int[(uint8_t)data.RC_arr[current]]);  // rcomp
                if (data.noise_len_arr[current] == 0)
                    raw_data[12][0].push_back(1);  // rtype = P
                else {
                    raw_data[12][0].push_back(3);  // rtype = M
                    for (uint16_t j = 0; j < data.noise_len_arr[current]; j++) {
                        raw_data[3][0].push_back(0);  // mmpos
                        if (j == 0)
                            raw_data[3][1].push_back(data.noisepos_arr[data.pos_in_noise_arr[current] + j]);  // mmpos
                        else
                            raw_data[3][1].push_back(data.noisepos_arr[data.pos_in_noise_arr[current] + j] -
                                                     1);  // mmpos
                        raw_data[4][0].push_back(0);      // mmtype = Substitution
                        raw_data[4][1].push_back(
                            char_to_int[(uint8_t)data.noise_arr[data.pos_in_noise_arr[current] + j]]);
                    }
                    raw_data[3][0].push_back(1);
                }
            } else {
                raw_data[12][0].push_back(5);                                 // rtype
                raw_data[7][0].push_back(data.read_length_arr[current] - 1);  // rlen
                for (uint64_t j = 0; j < data.read_length_arr[current]; j++) {
                    raw_data[6][0].push_back(
                        char_to_int[(uint8_t)data.unaligned_arr[data.pos_arr[current] + j]]);  // ureads
                }
                raw_data[0][0].push_back(seq_end - prevpos);                  // pos
                raw_data[1][0].push_back(0);                                  // rcomp
                raw_data[7][0].push_back(data.read_length_arr[current] - 1);  // rlen
                raw_data[12][0].push_back(1);                                 // rtype = P
                prevpos = seq_end;
                seq_end = prevpos + data.read_length_arr[current];
            }

            // pair subsequences
            bool same_block = (bdata.block_num[current] == bdata.block_num[pair]);
            if (same_block)
                stats->count_split_same_AU[cur_thread_num]++;
            else
                stats->count_split_diff_AU[cur_thread_num]++;

            bool read_1_first = (current < pair);
            if (same_block && !read_1_first) {
                raw_data[8][0].push_back(1);  // R1_split
                raw_data[8][2].push_back(bdata.genomic_record_index[pair]);
            } else if (same_block && read_1_first) {
                raw_data[8][0].push_back(2);  // R2_split
                raw_data[8][3].push_back(bdata.genomic_record_index[pair]);
            } else if (!same_block && !read_1_first) {
                raw_data[8][0].push_back(3);  // R1_diff_ref_seq
                raw_data[8][4].push_back(bdata.block_num[pair]);
                raw_data[8][6].push_back(bdata.genomic_record_index[pair]);
            } else {
                raw_data[8][0].push_back(4);  // R2_diff_ref_seq
                raw_data[8][5].push_back(bdata.block_num[pair]);
                raw_data[8][7].push_back(bdata.genomic_record_index[pair]);
            }
        }
    }
}

void generate_read_streams_pe(const std::string &temp_dir, const compression_params &cp, const std::vector<std::vector<gabac::EncodingConfiguration>> &configs) {
    // basic approach: start looking at reads from left to right. If current is
    // aligned but pair is unaligned, pair is kept at the end current AU and
    // stored in different record. We try to keep number of records in AU =
    // num_reads_per_block (without counting the the unaligned pairs above. As we
    // scan to right, if we come across a read whose pair has already been seen in
    // the same AU and the gap is < 32768 (and non-ovelapping since delta >= 0),
    // then we add this to the paired read's genomic record. Finally when we come
    // to unaligned reads whose pair is also unaligned, we store them in same
    // genomic record.

    se_data data;
    loadPE_Data(cp, temp_dir, &data);

    pe_block_data bdata;
    generateBlocksPE(data, &bdata);
    data.order_arr.clear();

    generate_qual_id_pe(temp_dir, bdata, cp.num_reads);

    pe_statistics stats;
    stats.count_same_rec = std::vector<uint32_t>(cp.num_thr, 0);
    stats.count_split_same_AU = std::vector<uint32_t>(cp.num_thr, 0);
    stats.count_split_diff_AU = std::vector<uint32_t>(cp.num_thr, 0);

    std::vector<uint32_t> num_reads_per_block(bdata.block_start.size());
    std::vector<uint32_t> num_records_per_block(bdata.block_start.size());

    // PE step 4: Now generate read streams and compress blocks in parallel
// this is actually number of read pairs per block for PE
#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(cp.num_thr) schedule(dynamic)
#endif
    for (uint64_t cur_block_num = 0; cur_block_num < bdata.block_start.size(); cur_block_num++) {
        auto raw_data = generate_empty_raw_data();

        generate_streams_pe(data, bdata, cur_block_num, raw_data, &stats);
        num_reads_per_block[cur_block_num] = raw_data[1][0].size(); // rcomp
        num_records_per_block[cur_block_num] = bdata.block_end[cur_block_num] - bdata.block_start[cur_block_num]; // used later for ids
        std::vector<std::vector<std::vector<gabac::DataBlock>>> generated_streams = create_default_streams();
        compress_read_subseqs(raw_data, generated_streams, configs);

        std::string file_to_save_streams = temp_dir + "/read_streams." + std::to_string(cur_block_num);
        write_streams_to_file(generated_streams, file_to_save_streams, read_descriptors);
    }  // end omp parallel

    std::cout << "count_same_rec: " << std::accumulate(stats.count_same_rec.begin(), stats.count_same_rec.end(), 0)
              << "\n";
    std::cout << "count_split_same_AU: "
              << std::accumulate(stats.count_split_same_AU.begin(), stats.count_split_same_AU.end(), 0) << "\n";
    std::cout << "count_split_diff_AU: "
              << std::accumulate(stats.count_split_diff_AU.begin(), stats.count_split_diff_AU.end(), 0) << "\n";

    // write num blocks, reads per block and records per block to a file
    const std::string block_info_file = temp_dir + "/block_info.bin";
    std::ofstream f_block_info(block_info_file, std::ios::binary);
    uint32_t num_blocks = bdata.block_start.size();
    f_block_info.write((char*)&num_blocks, sizeof(uint32_t));
    f_block_info.write((char*)&num_reads_per_block[0],num_blocks*sizeof(uint32_t));
    f_block_info.write((char*)&num_records_per_block[0],num_blocks*sizeof(uint32_t));
    return;
}

}  // namespace spring
