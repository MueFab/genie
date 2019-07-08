#include <array>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "util.h"
#include "decompress.h"
#include "id_tokenization.h"
#include "params.h"
#include <utils/fastq-record.h>

namespace spring {

    std::pair<std::vector<utils::FastqRecord>, std::vector<bool>>
    decode_streams(decoded_desc_t &dec, const std::vector<std::array<uint8_t, 2>> &subseq_indices, bool paired_end) {
        std::vector<utils::FastqRecord> decoded_records;
        std::vector<bool> first_file_flag_vec;
        std::string cur_read[2];
        std::string cur_quality[2];
        std::string cur_ID;
        utils::FastqRecord cur_record;
        std::string refBuf;
        // int_to_char
        char int_to_char[5] = {'A', 'C', 'G', 'T', 'N'};

        std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>::iterator>> subseq_it;
        // intialize iterators for subsequences
        for (auto arr : subseq_indices)
            subseq_it[arr[0]][arr[1]] = (dec.subseq_vector[arr[0]][arr[1]]).begin();
        uint32_t pos_in_tokens_array[128][8];
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 8; j++)
                pos_in_tokens_array[i][j] = 0;
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
                uint32_t rlen = (uint32_t) (*(subseq_it[7][0]++)); // rlen
                for (uint32_t i = 0; i < rlen; i++) {
                    refBuf.push_back(int_to_char[*(subseq_it[6][0]++)]); // ureads
                }
            } else {
                //get ID
                cur_ID = decode_id_tokens(prev_ID, prev_tokens_ptr, prev_tokens_len, dec.tokens, pos_in_tokens_array);
                // rtype can be 1 (P) or 3 (M)
                uint8_t number_of_record_segments;
                uint16_t delta = 0;
                bool read_1_first = true;
                if (paired_end) {
                    uint64_t pairing_decoding_case = (uint64_t) (*(subseq_it[8][0]++));
                    // note that we don't decode/use mateAUid and mateRecordIndex in this decoder
                    // the compressor still stores this information which can be a bit redundant
                    // given it is stored at two places and also the ids are stored at both places.
                    // one way to resolve this could be to use the R1_unpaired and R2_unpaired
                    // decoding cases, but we don't use those as they are not semantically correct
                    switch (pairing_decoding_case) {
                        case 0:
                            read_1_first = !(((uint16_t) (*(subseq_it[8][1]))) & 1);
                            delta = ((uint16_t) (*(subseq_it[8][1]++))) >> 1;
                            number_of_record_segments = 2;
                            break;
                        case 1:
                        case 3:
                        case 6:
                            number_of_record_segments = 1;
                            read_1_first = false;
                            break;
                        case 2:
                        case 4:
                        case 5:
                            number_of_record_segments = 1;
                            read_1_first = true;
                            break;
                        default:
                            throw std::runtime_error("Invalid pair decoding case encountered");
                    }
                } else {
                    number_of_record_segments = 1;
                }
                uint32_t rlen[2];
                for (int i = 0; i < number_of_record_segments; i++)
                    rlen[i] = (uint32_t) (*(subseq_it[7][0]++)); // rlen
                uint32_t pos = *(subseq_it[0][0]++); // pos

                abs_pos += pos;
                std::string cur_read[2];
                cur_read[0] = refBuf.substr(abs_pos, rlen[0]);
                if (number_of_record_segments == 2) {
                    uint64_t pair_abs_pos = abs_pos + delta;
                    cur_read[1] = refBuf.substr(pair_abs_pos, rlen[1]);
                }
                bool reverseComp[2];
                for (int i = 0; i < number_of_record_segments; i++)
                    reverseComp[i] = *(subseq_it[1][0]++); // rcomp
                if (rtype == 3) {
                    // type M
                    for (int i = 0; i < number_of_record_segments; i++) {
                        uint32_t abs_mmpos = 0;
                        while (true) {
                            bool mmpos_flag = (bool) (*(subseq_it[3][0]++));
                            if (mmpos_flag == 1)
                                break;
                            uint32_t mmpos = (uint32_t) (*(subseq_it[3][1]++));
                            abs_mmpos += mmpos;
                            uint32_t mmtype_0 = (uint32_t) (*(subseq_it[4][0]++));
                            if (mmtype_0 != 0) // i.e., not substitution
                                throw std::runtime_error("Non zero mmtype encountered.");
                            uint32_t mmtype_1 = (uint32_t) (*(subseq_it[4][1]++));
                            cur_read[i][abs_mmpos] = int_to_char[mmtype_1];
                            abs_mmpos++;
                        }
                    }
                }
                // finally, reverse complement if needed
                for (int i = 0; i < number_of_record_segments; i++) {
                    if (reverseComp[i] == 1)
                        cur_read[i] = reverse_complement(cur_read[i], rlen[i]);
                }

                for (int i = 0; i < number_of_record_segments; i++) {
                    // get quality
                    cur_quality[i] = dec.quality_arr.substr(pos_in_quality_arr, rlen[i]);
                    pos_in_quality_arr += rlen[i];
                }

                for (int i = 0; i < number_of_record_segments; i++) {
                    if (paired_end) {
                        if ((i == 0 && read_1_first) || (i == 1 && !read_1_first)) {
                            cur_record.title = cur_ID + "/1";
                            first_file_flag_vec.push_back(true);
                        } else {
                            cur_record.title = cur_ID + "/2";
                            first_file_flag_vec.push_back(false);
                        }
                    } else {
                        cur_record.title = cur_ID;
                    }
                    cur_record.sequence = cur_read[i];
                    cur_record.qualityScores = cur_quality[i];
                    decoded_records.push_back(cur_record);
                }
            }
        }
        return std::make_pair(decoded_records, first_file_flag_vec);
    }

    bool decompress(const std::string &temp_dir) {
        // decompress to temp_dir/decompressed.fastq

        std::string basedir = temp_dir;
        std::string file_subseq_prefix = basedir + "/subseq";
        compression_params *cp_ptr = new compression_params;
        compression_params &cp = *cp_ptr;
        // Read compression params
        std::string compression_params_file = temp_dir + "/cp.bin";
        std::ifstream f_cp(compression_params_file, std::ios::binary);
        if (!f_cp.is_open()) throw std::runtime_error("Can't open parameter file.");
        f_cp.read((char *) &cp, sizeof(compression_params));
        if (!f_cp.good())
            throw std::runtime_error("Can't read compression parameters.");
        f_cp.close();

        std::vector<std::array<uint8_t, 2>> subseq_indices = {
                {0,  0}, // pos
                {1,  0}, // rcomp
                {3,  0}, // mmpos
                {3,  1}, // mmpos
                {4,  0}, // mmtype
                {4,  1}, // mmtype
                {6,  0}, // ureads
                {7,  0}, // rlen
                {8,  0}, // pair
                {8,  1}, // pair
                {8,  2}, // pair
                {8,  3}, // pair
                {8,  4}, // pair
                {8,  5}, // pair
                {8,  6}, // pair
                {8,  7}, // pair
                {12, 0} // rtype
        };

        std::string file_quality = basedir + "/quality_1";
        std::string file_id = basedir + "/id_1";
        std::string file_decompressed_fastq = cp.paired_end ? basedir + "/decompressed_1.fastq" : basedir +
                                                                                                  "/decompressed.fastq";;
        std::string file_decompressed_fastq2 = basedir + "/decompressed_2.fastq";

        std::ofstream fout(file_decompressed_fastq);
        std::ofstream fout2;

        if (cp.paired_end) {
            fout2.open(file_decompressed_fastq2);
        }

        decoded_desc_t dec;
        for (uint32_t block_num = 0; block_num < cp.num_blocks; block_num++) {
            for (auto arr : subseq_indices) {
                std::string filename = file_subseq_prefix + "." + std::to_string(block_num) + "." +
                                       std::to_string(arr[0]) + "." + std::to_string(arr[1]);
                dec.subseq_vector[arr[0]][arr[1]] = read_vector_from_file(filename);
            }
            dec.quality_arr = read_file_as_string(file_quality + '.' + std::to_string(block_num));
            read_read_id_tokens_from_file(file_id + '.' + std::to_string(block_num), dec.tokens);
            auto decoded_records = decode_streams(dec, subseq_indices, cp.paired_end);
            for (size_t i = 0; i < decoded_records.first.size(); i++) {
                bool first_file_flag = true;
                if (cp.paired_end)
                    if (!decoded_records.second[i])
                        first_file_flag = false;
                std::ostream &tmpout = first_file_flag ? fout : fout2;
                tmpout << decoded_records.first[i].title << "\n";
                tmpout << decoded_records.first[i].sequence << "\n";
                tmpout << "+" << "\n";
                tmpout << decoded_records.first[i].qualityScores << "\n";
            }
        }
        bool paired_end = cp.paired_end;
        delete cp_ptr;

        return paired_end;
    }

}  // namespace spring
