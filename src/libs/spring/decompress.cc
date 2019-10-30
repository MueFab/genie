#include "decompress.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "id-tokenization.h"
#include "params.h"
#include "util.h"

namespace spring {

void decode_streams(decoded_desc_t &dec, bool paired_end, bool preserve_quality, bool preserve_id, bool combine_pairs,
                    std::vector<util::FastqRecord> matched_records[2],
                    std::vector<util::FastqRecord> unmatched_records[2], std::vector<uint32_t> &mate_au_id,
                    std::vector<uint32_t> &mate_record_index) {
    /*
     * return values are matched_records[2] (for pairs that are matched),
     * unmatched_records[2] (for pairs that are unmatched), mate_au_id,
     * mate_record_index (which store position of the pair of the
     * unmatched_records[1]). For single end case, only matched_records[0] is
     * populated, for paired end reads with combine_pairs false, only
     * matched_records[0] and matched_records[1] are populated (unmatched records
     * also put in these). For paired end with combine_pairs true, matched_records
     * arrays contain the matched records and have equal size, unmatched_records
     * have the records that don't have pair within the same AU, and mate_au_id &
     * mate_au_id contain the information needed to match them together.
     */
    std::vector<uint32_t> mate_record_index_same_rec;  // for sorting in the combine_pairs case
    std::vector<util::FastqRecord> unmatched_same_au[2];
    std::string cur_quality[2];
    std::string cur_ID;
    util::FastqRecord cur_record;
    std::string refBuf;
    // int_to_char
    char int_to_char[5] = {'A', 'C', 'G', 'T', 'N'};

    std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>::iterator>> subseq_it;
    // intialize iterators for subsequences
    for (auto arr : subseq_indices) subseq_it[arr[0]][arr[1]] = (dec.subseq_vector[arr[0]][arr[1]]).begin();
    uint32_t pos_in_tokens_array[128][6];
    for (int i = 0; i < 128; i++)
        for (int j = 0; j < 6; j++) pos_in_tokens_array[i][j] = 0;
    uint64_t pos_in_quality_arr = 0;
    std::string prev_ID;
    uint32_t prev_tokens_ptr[MAX_NUM_TOKENS_ID] = {0};
    uint32_t prev_tokens_len[MAX_NUM_TOKENS_ID] = {0};

    // some state variables
    uint64_t abs_pos = 0;

    for (; subseq_it[12][0] != dec.subseq_vector[12][0].end(); ++subseq_it[12][0]) {
        auto rtype = *subseq_it[12][0];
        if (rtype == 5) {
            // put in refBuf
            uint32_t rlen = (uint32_t)(*(subseq_it[7][0]++)) + 1;  // rlen
            for (uint32_t i = 0; i < rlen; i++) {
                refBuf.push_back(int_to_char[*(subseq_it[6][0]++)]);  // ureads
            }
        } else {
            if (preserve_id) {
                // get ID
                cur_ID = decode_id_tokens(prev_ID, prev_tokens_ptr, prev_tokens_len, dec.tokens, pos_in_tokens_array);
            } else {
                cur_ID = "@";
            }
            // rtype can be 1 (P) or 3 (M)
            uint8_t number_of_record_segments;
            uint16_t delta = 0;
            bool read_1_first = true;
            bool same_au_flag = false;  // when combine_pairs true and pair is split:
                                        // flag to indicate if mate in same AU
            if (paired_end) {
                uint64_t pairing_decoding_case = (uint64_t)(*(subseq_it[8][0]++));
                // note that we don't decode/use mateAUid and mateRecordIndex in this
                // decoder the compressor still stores this information which can be a
                // bit redundant given it is stored at two places and also the ids are
                // stored at both places. one way to resolve this could be to use the
                // R1_unpaired and R2_unpaired decoding cases, but we don't use those as
                // they are not semantically correct
                switch (pairing_decoding_case) {
                    case 0:
                        read_1_first = !(((uint16_t)(*(subseq_it[8][1]))) & 1);
                        delta = ((uint16_t)(*(subseq_it[8][1]++))) >> 1;
                        number_of_record_segments = 2;
                        break;
                    case 1:
                    case 3:
                    case 6:
                        number_of_record_segments = 1;
                        read_1_first = false;
                        if (combine_pairs) {
                            if (pairing_decoding_case == 1) {
                                mate_record_index_same_rec.push_back((uint32_t)(*(subseq_it[8][2]++)));
                                same_au_flag = true;
                            } else if (pairing_decoding_case == 3) {
                                mate_au_id.push_back((uint32_t)(*(subseq_it[8][4]++)));
                                mate_record_index.push_back((uint32_t)(*(subseq_it[8][6]++)));
                                same_au_flag = false;
                            }
                        }
                        break;
                    case 2:
                    case 4:
                    case 5:
                        number_of_record_segments = 1;
                        read_1_first = true;
                        if (combine_pairs) {
                            if (pairing_decoding_case == 2)
                                same_au_flag = true;
                            else if (pairing_decoding_case == 4)
                                same_au_flag = false;
                        }
                        break;
                    default:
                        throw std::runtime_error("Invalid pair decoding case encountered");
                }
            } else {
                number_of_record_segments = 1;
            }
            uint32_t rlen[2];
            for (int i = 0; i < number_of_record_segments; i++) rlen[i] = (uint32_t)(*(subseq_it[7][0]++)) + 1;  // rlen
            uint32_t pos = *(subseq_it[0][0]++);                                                                 // pos

            abs_pos += pos;
            std::string cur_read[2];
            cur_read[0] = refBuf.substr(abs_pos, rlen[0]);
            if (number_of_record_segments == 2) {
                uint64_t pair_abs_pos = abs_pos + delta;
                cur_read[1] = refBuf.substr(pair_abs_pos, rlen[1]);
            }
            bool reverseComp[2];
            for (int i = 0; i < number_of_record_segments; i++) reverseComp[i] = *(subseq_it[1][0]++);  // rcomp
            if (rtype == 3) {
                // type M
                for (int i = 0; i < number_of_record_segments; i++) {
                    uint32_t abs_mmpos = 0;
                    while (true) {
                        bool mmpos_flag = (bool)(*(subseq_it[3][0]++));
                        if (mmpos_flag == 1) break;
                        uint32_t mmpos = (uint32_t)(*(subseq_it[3][1]++));
                        abs_mmpos += mmpos;
                        uint32_t mmtype_0 = (uint32_t)(*(subseq_it[4][0]++));
                        if (mmtype_0 != 0)  // i.e., not substitution
                            throw std::runtime_error("Non zero mmtype encountered.");
                        uint32_t mmtype_1 = (uint32_t)(*(subseq_it[4][1]++));
                        cur_read[i][abs_mmpos] = int_to_char[mmtype_1];
                        abs_mmpos++;
                    }
                }
            }
            // finally, reverse complement if needed
            for (int i = 0; i < number_of_record_segments; i++) {
                if (reverseComp[i] == 1) cur_read[i] = reverse_complement(cur_read[i], rlen[i]);
            }
            if (preserve_quality) {
                for (int i = 0; i < number_of_record_segments; i++) {
                    // get quality
                    cur_quality[i] = dec.quality_arr.substr(pos_in_quality_arr, rlen[i]);
                    pos_in_quality_arr += rlen[i];
                }
            }
            bool first_read_flag = true;
            for (int i = 0; i < number_of_record_segments; i++) {
                if (paired_end) {
                    if ((i == 0 && read_1_first) || (i == 1 && !read_1_first)) {
                        cur_record.title = cur_ID + "/1";
                        first_read_flag = true;
                    } else {
                        cur_record.title = cur_ID + "/2";
                        first_read_flag = false;
                    }
                } else {
                    cur_record.title = cur_ID;
                }
                cur_record.sequence = cur_read[i];
                cur_record.qualityScores = cur_quality[i];
                if (!paired_end) {
                    matched_records[0].push_back(cur_record);
                } else {
                    if (!combine_pairs) {
                        matched_records[first_read_flag ? 0 : 1].push_back(cur_record);
                    } else {
                        if (number_of_record_segments == 2) {
                            matched_records[first_read_flag ? 0 : 1].push_back(cur_record);
                        } else {
                            if (same_au_flag)
                                unmatched_same_au[first_read_flag ? 0 : 1].push_back(cur_record);
                            else
                                unmatched_records[first_read_flag ? 0 : 1].push_back(cur_record);
                        }
                    }
                }
            }
        }
    }
    // if combine_pairs is true, reorder reads in unmatched_same_au so that pairs
    // match, and push this to matched_records arrays
    if (paired_end && combine_pairs) {
        size_t size_unmatched = unmatched_same_au[0].size();
        if (size_unmatched != unmatched_same_au[1].size() || size_unmatched != mate_record_index_same_rec.size())
            throw std::runtime_error("Sizes of unmatched same AU vectors don't match.");
        if (size_unmatched > 0) {
            matched_records[0].insert(matched_records[0].end(), unmatched_same_au[0].begin(),
                                      unmatched_same_au[0].end());
            std::vector<std::pair<uint32_t, uint32_t>> record_index_for_sorting(size_unmatched);
            for (size_t i = 0; i < size_unmatched; i++)
                record_index_for_sorting[i] = std::make_pair(mate_record_index_same_rec[i], i);
            std::sort(
                record_index_for_sorting.begin(), record_index_for_sorting.end(),
                [](std::pair<uint32_t, uint32_t> a, std::pair<uint32_t, uint32_t> b) { return a.first < b.first; });
            for (size_t i = 0; i < size_unmatched; i++)
                matched_records[1].push_back(unmatched_same_au[1][record_index_for_sorting[i].second]);
        }
    }
    return;
}

void decode_streams_ureads(decoded_desc_t &dec, bool paired_end, bool preserve_quality, bool preserve_id,
                           std::vector<util::FastqRecord> matched_records[2]) {
    /*
     * return value is matched_records[2]. For single end case, only
     * matched_records[0] is populated, for paired end reads matched_records[0]
     * and matched_records[1] are populated.
     */
    for (int j = 0; j < 2; j++) {
        matched_records[j].clear();
    }
    std::string cur_quality[2];
    std::string cur_ID;
    util::FastqRecord cur_record;
    // int_to_char
    char int_to_char[5] = {'A', 'C', 'G', 'T', 'N'};

    std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>::iterator>> subseq_it;
    // intialize iterators for subsequences
    for (auto arr : subseq_indices) subseq_it[arr[0]][arr[1]] = (dec.subseq_vector[arr[0]][arr[1]]).begin();
    uint32_t pos_in_tokens_array[128][6];
    for (int i = 0; i < 128; i++)
        for (int j = 0; j < 6; j++) pos_in_tokens_array[i][j] = 0;
    uint64_t pos_in_quality_arr = 0;
    std::string prev_ID;
    uint32_t prev_tokens_ptr[MAX_NUM_TOKENS_ID] = {0};
    uint32_t prev_tokens_len[MAX_NUM_TOKENS_ID] = {0};
    uint8_t number_of_record_segments = paired_end ? 2 : 1;
    while (subseq_it[7][0] != dec.subseq_vector[7][0].end()) {
        uint32_t rlen[2];
        for (int i = 0; i < number_of_record_segments; i++) rlen[i] = (uint32_t)(*(subseq_it[7][0]++)) + 1;  // rlen

        std::string cur_read[2];
        for (int i = 0; i < number_of_record_segments; i++) {
            for (uint32_t j = 0; j < rlen[i]; j++) {
                cur_read[i].push_back(int_to_char[*(subseq_it[6][0]++)]);  // ureads
            }
        }
        if (preserve_id) {
            // get ID
            cur_ID = decode_id_tokens(prev_ID, prev_tokens_ptr, prev_tokens_len, dec.tokens, pos_in_tokens_array);
        } else {
            cur_ID = "@";
        }
        if (preserve_quality) {
            for (int i = 0; i < number_of_record_segments; i++) {
                // get quality
                cur_quality[i] = dec.quality_arr.substr(pos_in_quality_arr, rlen[i]);
                pos_in_quality_arr += rlen[i];
            }
        }
        for (int i = 0; i < number_of_record_segments; i++) {
            cur_record.title = cur_ID;
            if (!paired_end && i == 1) {
                break;
            } else {
                if (paired_end) {
                    if (i == 0) {
                        cur_record.title += "/1";
                    } else {
                        cur_record.title += "/2";
                    }
                }
            }
            cur_record.sequence = cur_read[i];
            cur_record.qualityScores = cur_quality[i];
            matched_records[i].push_back(cur_record);
        }
    }
    return;
}

bool decompress(const std::string &temp_dir, dsg::StreamSaver *ld, int num_thr, bool combine_pairs) {
    // decompress to temp_dir/decompressed.fastq

    std::string basedir = temp_dir;
    compression_params *cp_ptr = new compression_params;
    compression_params &cp = *cp_ptr;
    // Read compression params
    std::string compression_params_file = "cp.bin";

    gabac::DataBlock tmp(0, 1);
    ld->unpack(compression_params_file, &tmp);
    ld->decompress(compression_params_file, &tmp);
    std::memcpy(&cp, tmp.getData(), sizeof(compression_params));
    tmp.clear();
    std::string file_quality = "quality_1";
    std::string file_id = "id_1";
    std::string file_decompressed_fastq =
        cp.paired_end ? basedir + "/decompressed_1.fastq" : basedir + "/decompressed.fastq";
    ;
    std::string file_decompressed_fastq2 = basedir + "/decompressed_2.fastq";
    // temporary files for combine pairs case
    std::string file_unmatched_fastq1 = basedir + "/unmatched_1.fastq";
    std::string file_unmatched_fastq2 = basedir + "/unmatched_2.fastq";
    std::ofstream fout(file_decompressed_fastq);
    std::ofstream fout2;
    std::ofstream fout_unmatched1, fout_unmatched2;
    if (cp.paired_end) {
        fout2.open(file_decompressed_fastq2);
        if (combine_pairs && !cp.ureads_flag) {
            fout_unmatched1.open(file_unmatched_fastq1);
            fout_unmatched2.open(file_unmatched_fastq2);
        }
    }
    std::vector<util::FastqRecord>(*matched_records)[2] = new std::vector<util::FastqRecord>[num_thr][2];
    std::vector<util::FastqRecord>(*unmatched_records)[2] = new std::vector<util::FastqRecord>[num_thr][2];
    std::vector<uint32_t> *mate_au_id = new std::vector<uint32_t>[num_thr];
    std::vector<uint32_t> *mate_record_index = new std::vector<uint32_t>[num_thr];

    // store vectors to hold the AUid and indices which will be used later for
    // combining.
    std::vector<uint32_t> mate_au_id_concat, mate_record_index_concat;

#ifdef GENIE_USE_OPENMP
#pragma omp parallel for ordered num_threads(num_thr) schedule(dynamic)
#endif
    for (uint32_t block_num = 0; block_num < cp.num_blocks; block_num++) {
        dsg::AcessUnitStreams sstreams;
        dsg::AcessUnitStreams tstreams;
        gabac::DataBlock qualityBlock(0, 1);
        decoded_desc_t dec;
#ifdef GENIE_USE_OPENMP
        int tid = omp_get_thread_num();
#pragma omp critical
#else
        int tid = 0;
#endif
        {
            for (auto arr : subseq_indices) {
                std::string filename = file_subseq_prefix + "." + std::to_string(block_num) + "." +
                                       std::to_string(arr[0]) + "." + std::to_string(arr[1]);
                dec.subseq_vector[arr[0]][arr[1]] = read_vector_from_file(filename);
                sstreams.streams[arr[0]][arr[1]] = gabac::DataBlock(0, 1);
                ld->unpack(filename, &sstreams.streams[arr[0]][arr[1]]);
            }
            if (cp.preserve_quality) {
                ld->unpack(file_quality + '.' + std::to_string(block_num), &qualityBlock);
            }
            if (cp.preserve_id) {
                for (int i = 0; i < 128; i++) {
                    for (int j = 0; j < 6; j++) {
                        std::string infile_name_i_j = file_id + '.' + std::to_string(block_num) + "." +
                                                      std::to_string(i) + "." + std::to_string(j);
                        ld->unpack(infile_name_i_j, &tstreams.streams[i][j]);
                    }
                }
            }
        }

        // Decompression gabac start

        for (auto arr : subseq_indices) {
            std::string filename = file_subseq_prefix + "." + std::to_string(block_num) + "." + std::to_string(arr[0]) +
                                   "." + std::to_string(arr[1]);
            ld->decompress(filename, &sstreams.streams[arr[0]][arr[1]]);
            dec.subseq_vector[arr[0]][arr[1]].resize(sstreams.streams[arr[0]][arr[1]].getRawSize() / sizeof(int64_t));
            std::memcpy(dec.subseq_vector[arr[0]][arr[1]].data(), sstreams.streams[arr[0]][arr[1]].getData(),
                        sstreams.streams[arr[0]][arr[1]].getRawSize());
            sstreams.streams[arr[0]][arr[1]].clear();
        }
        if (cp.preserve_quality) {
            ld->decompress(file_quality + '.' + std::to_string(block_num), &qualityBlock);
            dec.quality_arr = std::string(qualityBlock.getRawSize(), '\0');
            std::memcpy(const_cast<char *>(dec.quality_arr.data()), qualityBlock.getData(), qualityBlock.getRawSize());
            qualityBlock.clear();
        }

        if (cp.preserve_id) {
            for (int i = 0; i < 128; i++) {
                for (int j = 0; j < 6; j++) {
                    std::string infile_name_i_j =
                        file_id + '.' + std::to_string(block_num) + "." + std::to_string(i) + "." + std::to_string(j);

                    if (!tstreams.streams[i][j].empty()) {
                        ld->decompress(infile_name_i_j, &tstreams.streams[i][j]);
                        dec.tokens[i][j].resize(tstreams.streams[i][j].getRawSize() / sizeof(int64_t));
                        std::memcpy(dec.tokens[i][j].data(), tstreams.streams[i][j].getData(),
                                    tstreams.streams[i][j].getRawSize());
                        tstreams.streams[i][j].clear();
                    } else {
                        dec.tokens[i][j].clear();
                    }
                }
            }
        }

        // Decompression gabac end
        if (cp.ureads_flag) {
            decode_streams_ureads(dec, cp.paired_end, cp.preserve_quality, cp.preserve_id, matched_records[tid]);
        } else {
            decode_streams(dec, cp.paired_end, cp.preserve_quality, cp.preserve_id, combine_pairs, matched_records[tid],
                           unmatched_records[tid], mate_au_id[tid], mate_record_index[tid]);
        }
#ifdef GENIE_USE_OPENMP
#pragma omp ordered
#endif
        {
            for (int j = 0; j < 2; j++) {
                if (j == 1 && !cp.paired_end) break;
                std::ostream &tmpout = (j == 0) ? fout : fout2;
                for (auto fastqRecord : matched_records[tid][j])
                    write_fastq_record_to_ostream(tmpout, fastqRecord, cp.preserve_quality);
            }
            if (cp.paired_end && combine_pairs) {
                for (int j = 0; j < 2; j++) {
                    std::ostream &tmpout = (j == 0) ? fout_unmatched1 : fout_unmatched2;
                    for (auto fastqRecord : unmatched_records[tid][j])
                        write_fastq_record_to_ostream(tmpout, fastqRecord, cp.preserve_quality);
                }
                mate_au_id_concat.insert(mate_au_id_concat.end(), mate_au_id[tid].begin(), mate_au_id[tid].end());
                mate_record_index_concat.insert(mate_record_index_concat.end(), mate_record_index[tid].begin(),
                                                mate_record_index[tid].end());
            }
        }

        for (int j = 0; j < 2; j++) {
            matched_records[tid][j].clear();
            unmatched_records[tid][j].clear();
        }
        mate_au_id[tid].clear();
        mate_record_index[tid].clear();
    }

    // now reorder the remaining unmatched reads so that they are paired and then
    // write them to file.
    if (!cp.ureads_flag && cp.paired_end && combine_pairs) {
        fout_unmatched1.close();
        fout_unmatched2.close();
        // first write the reads in file 1 as it is
        std::ifstream fin_unmatched1(file_unmatched_fastq1);
        fout << fin_unmatched1.rdbuf();
        fin_unmatched1.close();
        fout.clear();  // clear error flags if f_unmatched1 is empty

        // now build index and reverse index to reorder reads in file 2
        size_t size_unmatched = mate_au_id_concat.size();
        if (size_unmatched > 0) {
            std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> au_id_index_for_sorting(size_unmatched);
            for (size_t i = 0; i < size_unmatched; i++)
                au_id_index_for_sorting[i] = std::make_tuple(mate_au_id_concat[i], mate_record_index_concat[i], i);
            std::sort(au_id_index_for_sorting.begin(), au_id_index_for_sorting.end(),
                      [](std::tuple<uint32_t, uint32_t, uint32_t> a, std::tuple<uint32_t, uint32_t, uint32_t> b) {
                          return (std::get<0>(a) == std::get<0>(b)) ? (std::get<1>(a) < std::get<1>(b))
                                                                    : (std::get<0>(a) < std::get<0>(b));
                      });
            std::vector<uint32_t> reverse_index(size_unmatched);
            for (size_t i = 0; i < size_unmatched; i++) reverse_index[std::get<2>(au_id_index_for_sorting[i])] = i;

            // now reorder the unmatched records in file 2, by picking them in chunks
            uint32_t bin_size = std::min((size_t)BIN_SIZE_COMBINE_PAIRS, size_unmatched);
            std::vector<util::FastqRecord> records_bin(bin_size);
            util::FastqRecord tmpFastqRecord;
            for (uint32_t i = 0; i <= size_unmatched / bin_size; i++) {
                uint32_t num_records_bin = bin_size;
                if (i == size_unmatched / bin_size) num_records_bin = size_unmatched % bin_size;
                if (num_records_bin == 0) break;  // we are done
                std::ifstream fin_unmatched2(file_unmatched_fastq2);
                uint32_t bin_start = i * bin_size;
                for (uint32_t j = 0; j < size_unmatched; j++) {
                    if (reverse_index[j] >= bin_start && reverse_index[j] < bin_start + num_records_bin)
                        read_fastq_record_from_ifstream(fin_unmatched2, records_bin[reverse_index[j] - bin_start],
                                                        cp.preserve_quality);
                    else
                        read_fastq_record_from_ifstream(fin_unmatched2, tmpFastqRecord, cp.preserve_quality);
                }
                fin_unmatched2.close();
                for (uint32_t j = 0; j < num_records_bin; j++)
                    write_fastq_record_to_ostream(fout2, records_bin[j], cp.preserve_quality);
            }
        }
    }
    fout.close();
    if (cp.paired_end) fout2.close();

    bool paired_end = cp.paired_end;
    delete cp_ptr;

    delete[] matched_records;
    delete[] unmatched_records;
    delete[] mate_au_id;
    delete[] mate_record_index;
    return paired_end;
}

void write_fastq_record_to_ostream(std::ostream &out, util::FastqRecord &fastqRecord, bool preserve_quality) {
    out << fastqRecord.title << "\n";
    out << fastqRecord.sequence << "\n";
    if (preserve_quality) {
        out << "+"
            << "\n";
        out << fastqRecord.qualityScores << "\n";
    }
}

void read_fastq_record_from_ifstream(std::ifstream &in, util::FastqRecord &fastqRecord, bool preserve_quality) {
    std::getline(in, fastqRecord.title);
    std::getline(in, fastqRecord.sequence);
    if (preserve_quality) {
        std::getline(in, fastqRecord.qualityScores);
        std::getline(in, fastqRecord.qualityScores);
    }
}

}  // namespace spring
