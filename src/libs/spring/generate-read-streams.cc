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
#include <format/mpegg_p2/clutter.h>

namespace spring {


    uint64_t getNumBlocks(const compression_params& data) {
        return uint64_t(std::ceil(float(data.num_reads) / data.num_reads_per_block));
    }

    std::unique_ptr<MpeggRawAu> generate_subseqs(const se_data &data, uint64_t block_num) {
        auto rdata = util::make_unique<MpeggRawAu>(util::make_unique<format::mpegg_p2::ParameterSet>(), 0);

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
        rdata->get(GenomicDescriptor::RLEN, GenomicSubsequence::RLEN).push(seq_end - seq_start - 1);  // rlen
        rdata->get(GenomicDescriptor::RTYPE, GenomicSubsequence::RTYPE).push(5);                       // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
            rdata->get(GenomicDescriptor::UREADS, GenomicSubsequence::UREADS).push(char_to_int[(uint8_t)data.seq[i]]);  // ureads
    }
    uint64_t prevpos = 0, diffpos;
    // Write streams
    for (uint64_t i = start_read_num; i < end_read_num; i++) {
        if (data.flag_arr[i] == true) {
            rdata->get(GenomicDescriptor::RLEN, GenomicSubsequence::RLEN).push(data.read_length_arr[i] - 1);         // rlen
            rdata->get(GenomicDescriptor::RCOMP, GenomicSubsequence::RCOMP).push(rc_to_int[(uint8_t)data.RC_arr[i]]);  // rcomp
            if (i == start_read_num) {
                // Note: In order non-preserving mode, if the first read of
                // the block is a singleton, then the rest are too.
                rdata->get(GenomicDescriptor::POS, 0).push(0);  // pos
                prevpos = data.pos_arr[i];
            } else {
                diffpos = data.pos_arr[i] - prevpos;
                rdata->get(GenomicDescriptor::POS, 0).push(diffpos);  // pos
                prevpos = data.pos_arr[i];
            }
            if (data.noise_len_arr[i] == 0)
                rdata->get(GenomicDescriptor::RTYPE, 0).push(1);  // rtype = P
            else {
                rdata->get(GenomicDescriptor::RTYPE, 0).push(3);  // rtype = M
                for (uint16_t j = 0; j < data.noise_len_arr[i]; j++) {
                    rdata->get(GenomicDescriptor::MMPOS, 0).push(0);  // mmpos
                    if (j == 0)
                        rdata->get(GenomicDescriptor::MMPOS, 1).push(data.noisepos_arr[data.pos_in_noise_arr[i] + j]);
                    else
                        rdata->get(GenomicDescriptor::MMPOS, 1).push(data.noisepos_arr[data.pos_in_noise_arr[i] + j] -
                                                 1);  // decoder adds +1
                    rdata->get(GenomicDescriptor::MMTYPE, 0).push(0);      // mmtype = Substitution
                    rdata->get(GenomicDescriptor::MMTYPE, 1).push(char_to_int[(uint8_t)data.noise_arr[data.pos_in_noise_arr[i] + j]]);
                }
                rdata->get(GenomicDescriptor::MMPOS, 0).push(1);
            }
        } else {
            rdata->get(GenomicDescriptor::RTYPE, 0).push(5);                           // rtype
            rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[i] - 1);  // rlen
            for (uint64_t j = 0; j < data.read_length_arr[i]; j++) {
                rdata->get(GenomicDescriptor::UREADS, 0).push(char_to_int[(uint8_t)data.unaligned_arr[data.pos_arr[i] + j]]);  // ureads
            }
            rdata->get(GenomicDescriptor::POS, 0).push(seq_end - prevpos);            // pos
            rdata->get(GenomicDescriptor::RCOMP, 0).push(0);                            // rcomp
            rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[i] - 1);  // rlen
            rdata->get(GenomicDescriptor::RTYPE, 0).push(1);                           // rtype = P
            prevpos = seq_end;
            seq_end = prevpos + data.read_length_arr[i];
        }
    }
    return rdata;
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

std::unique_ptr<MpeggRawAu> generate_streams_pe(const se_data &data, const pe_block_data &bdata, uint64_t cur_block_num, pe_statistics *pest) {
#ifdef GENIE_USE_OPENMP
    const unsigned cur_thread_num = omp_get_thread_num();
#else
    const unsigned cur_thread_num = 0;
#endif

    auto rdata = util::make_unique<MpeggRawAu>(util::make_unique<format::mpegg_p2::ParameterSet>(), 0);

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
        rdata->get(GenomicDescriptor::RLEN, 0).push(seq_end - seq_start - 1);  // rlen
        rdata->get(GenomicDescriptor::RTYPE, 0).push(5);                       // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
            rdata->get(GenomicDescriptor::UREADS, 0).push(char_to_int[(uint8_t)data.seq[i]]);  // ureads
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
                rdata->get(GenomicDescriptor::POS, 0).push(0);  // pos
                prevpos = data.pos_arr[current];
            } else {
                diffpos = data.pos_arr[current] - prevpos;
                rdata->get(GenomicDescriptor::POS, 0).push(diffpos);  // pos
                prevpos = data.pos_arr[current];
            }
        }
        if ((bdata.block_num[current] == bdata.block_num[pair]) &&
            (bdata.genomic_record_index[current] == bdata.genomic_record_index[pair])) {
            // both reads in same record
            if (data.flag_arr[current] == false) {
                // Case 1: both unaligned
                rdata->get(GenomicDescriptor::RTYPE, 0).push(5);                                                              // rtype
                rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[current] + data.read_length_arr[pair] - 1);  // rlen
                for (uint64_t j = 0; j < data.read_length_arr[current]; j++) {
                    rdata->get(GenomicDescriptor::UREADS, 0).push(
                        char_to_int[(uint8_t)data.unaligned_arr[data.pos_arr[current] + j]]);  // ureads
                }
                for (uint64_t j = 0; j < data.read_length_arr[pair]; j++) {
                    rdata->get(GenomicDescriptor::UREADS, 0).push(
                        char_to_int[(uint8_t)data.unaligned_arr[data.pos_arr[pair] + j]]);  // ureads
                }
                rdata->get(GenomicDescriptor::POS, 0).push(seq_end - prevpos);                  // pos
                rdata->get(GenomicDescriptor::RCOMP, 0).push(0);                                  // rcomp
                rdata->get(GenomicDescriptor::RCOMP, 0).push(0);                                  // rcomp
                rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[current] - 1);  // rlen
                rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[pair] - 1);     // rlen
                rdata->get(GenomicDescriptor::RTYPE, 0).push(1);                                 // rtype = P
                rdata->get(GenomicDescriptor::PAIR, 0).push(0);                                  // pair decoding case same_rec
                bool read_1_first = true;
                uint16_t delta = data.read_length_arr[current];
                rdata->get(GenomicDescriptor::PAIR, 1).push(!(read_1_first) + 2 * delta);  // pair
                prevpos = seq_end;
                seq_end = prevpos + data.read_length_arr[current] + data.read_length_arr[pair];
            } else {
                // Case 2: both aligned
                rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[current] - 1);         // rlen
                rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[pair] - 1);            // rlen
                rdata->get(GenomicDescriptor::RCOMP, 0).push(rc_to_int[(uint8_t)data.RC_arr[current]]);  // rcomp
                rdata->get(GenomicDescriptor::RCOMP, 0).push(rc_to_int[(uint8_t)data.RC_arr[pair]]);     // rcomp
                if (data.noise_len_arr[current] == 0 && data.noise_len_arr[pair] == 0)
                    rdata->get(GenomicDescriptor::RTYPE, 0).push(1);  // rtype = P
                else {
                    rdata->get(GenomicDescriptor::RTYPE, 0).push(3);  // rtype = M
                    for (int k = 0; k < 2; k++) {
                        uint32_t index = k ? pair : current;
                        for (uint16_t j = 0; j < data.noise_len_arr[index]; j++) {
                            rdata->get(GenomicDescriptor::MMPOS, 0).push(0);  // mmpos
                            if (j == 0)
                                rdata->get(GenomicDescriptor::MMPOS, 1).push(data.noisepos_arr[data.pos_in_noise_arr[index] + j]);  // mmpos
                            else
                                rdata->get(GenomicDescriptor::MMPOS, 1).push(data.noisepos_arr[data.pos_in_noise_arr[index] + j] -
                                                         1);  // mmpos
                            rdata->get(GenomicDescriptor::MMTYPE, 0).push(0);      // mmtype = Substitution
                            rdata->get(GenomicDescriptor::MMTYPE, 1).push(
                                char_to_int[(uint8_t)data.noise_arr[data.pos_in_noise_arr[index] + j]]);
                        }
                        rdata->get(GenomicDescriptor::MMPOS, 0).push(1);  // mmpos
                    }
                }
                bool read_1_first = (current < pair);
                uint16_t delta = data.pos_arr[pair] - data.pos_arr[current];
                rdata->get(GenomicDescriptor::PAIR, 0).push(0);                            // pair decoding case same_rec
                rdata->get(GenomicDescriptor::PAIR, 1).push(!(read_1_first) + 2 * delta);  // pair
                pest->count_same_rec[cur_thread_num]++;
            }
        } else {
            // only one read in genomic record
            if (data.flag_arr[current] == true) {
                rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[current] - 1);         // rlen
                rdata->get(GenomicDescriptor::RCOMP, 0).push(rc_to_int[(uint8_t)data.RC_arr[current]]);  // rcomp
                if (data.noise_len_arr[current] == 0)
                    rdata->get(GenomicDescriptor::RTYPE, 0).push(1);  // rtype = P
                else {
                    rdata->get(GenomicDescriptor::RTYPE, 0).push(3);  // rtype = M
                    for (uint16_t j = 0; j < data.noise_len_arr[current]; j++) {
                        rdata->get(GenomicDescriptor::MMPOS, 0).push(0);  // mmpos
                        if (j == 0)
                            rdata->get(GenomicDescriptor::MMPOS, 1).push(data.noisepos_arr[data.pos_in_noise_arr[current] + j]);  // mmpos
                        else
                            rdata->get(GenomicDescriptor::MMPOS, 1).push(data.noisepos_arr[data.pos_in_noise_arr[current] + j] -
                                                     1);  // mmpos
                        rdata->get(GenomicDescriptor::MMTYPE, 0).push(0);      // mmtype = Substitution
                        rdata->get(GenomicDescriptor::MMTYPE, 1).push(
                            char_to_int[(uint8_t)data.noise_arr[data.pos_in_noise_arr[current] + j]]);
                    }
                    rdata->get(GenomicDescriptor::MMPOS, 0).push(1);
                }
            } else {
                rdata->get(GenomicDescriptor::RTYPE, 0).push(5);                                 // rtype
                rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[current] - 1);  // rlen
                for (uint64_t j = 0; j < data.read_length_arr[current]; j++) {
                    rdata->get(GenomicDescriptor::UREADS, 0).push(
                        char_to_int[(uint8_t)data.unaligned_arr[data.pos_arr[current] + j]]);  // ureads
                }
                rdata->get(GenomicDescriptor::POS, 0).push(seq_end - prevpos);                  // pos
                rdata->get(GenomicDescriptor::RCOMP, 0).push(0);                                  // rcomp
                rdata->get(GenomicDescriptor::RLEN, 0).push(data.read_length_arr[current] - 1);  // rlen
                rdata->get(GenomicDescriptor::RTYPE, 0).push(1);                                 // rtype = P
                prevpos = seq_end;
                seq_end = prevpos + data.read_length_arr[current];
            }

            // pair subsequences
            bool same_block = (bdata.block_num[current] == bdata.block_num[pair]);
            if (same_block)
                pest->count_split_same_AU[cur_thread_num]++;
            else
                pest->count_split_diff_AU[cur_thread_num]++;

            bool read_1_first = (current < pair);
            if (same_block && !read_1_first) {
                rdata->get(GenomicDescriptor::PAIR, 0).push(1);  // R1_split
                rdata->get(GenomicDescriptor::PAIR, 2).push(bdata.genomic_record_index[pair]);
            } else if (same_block && read_1_first) {
                rdata->get(GenomicDescriptor::PAIR, 0).push(2);  // R2_split
                rdata->get(GenomicDescriptor::PAIR, 3).push(bdata.genomic_record_index[pair]);
            } else if (!same_block && !read_1_first) {
                rdata->get(GenomicDescriptor::PAIR, 0).push(3);  // R1_diff_ref_seq
                rdata->get(GenomicDescriptor::PAIR, 4).push(bdata.block_num[pair]);
                rdata->get(GenomicDescriptor::PAIR, 6).push(bdata.genomic_record_index[pair]);
            } else {
                rdata->get(GenomicDescriptor::PAIR, 0).push(4);  // R2_diff_ref_seq
                rdata->get(GenomicDescriptor::PAIR, 5).push(bdata.block_num[pair]);
                rdata->get(GenomicDescriptor::PAIR, 7).push(bdata.genomic_record_index[pair]);
            }
        }
    }

    return rdata;
}


}  // namespace spring
