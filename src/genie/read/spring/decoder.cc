/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "params.h"
#include "util.h"

namespace genie {
namespace read {
namespace spring {
void decode_streams(core::AccessUnit& au, bool paired_end, bool combine_pairs,
                    std::array<std::vector<Record>, 2>& matched_records,
                    std::array<std::vector<Record>, 2>& unmatched_records, std::vector<uint32_t> &mate_au_id,
                    std::vector<uint32_t> &mate_record_index, std::vector<std::string>& names, std::vector<std::string>& qvs) {
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
    std::vector<Record> unmatched_same_au[2];
    size_t qv_pos = 0;
    size_t name_pos = 0;
    Record cur_record;
    std::string refBuf;
    // int_to_char
    char int_to_char[5] = {'A', 'C', 'G', 'T', 'N'};

    // some state variables
    uint64_t abs_pos = 0;

    while(!au.get(core::GenSub::RTYPE).end()) {
        auto rtype = au.get(core::GenSub::RTYPE).pull();
        if (rtype == 5) {
            // put in refBuf
            uint32_t rlen = au.get(core::GenSub::RLEN).pull() + 1;  // rlen
            for (uint32_t i = 0; i < rlen; i++) {
                refBuf.push_back(int_to_char[au.get(core::GenSub::UREADS).pull()]);  // ureads
            }
        } else {
            // rtype can be 1 (P) or 3 (M)
            uint8_t number_of_record_segments;
            uint16_t delta = 0;
            bool read_1_first = true;
            bool same_au_flag = false;  // when combine_pairs true and pair is split:
                                        // flag to indicate if mate in same AU
            if (paired_end) {
                uint64_t pairing_decoding_case = au.get(core::GenSub::PAIR_DECODING_CASE).pull();
                // note that we don't decode/use mateAUid and mateRecordIndex in this
                // decoder the compressor still stores this information which can be a
                // bit redundant given it is stored at two places and also the ids are
                // stored at both places. one way to resolve this could be to use the
                // R1_unpaired and R2_unpaired decoding cases, but we don't use those as
                // they are not semantically correct
                uint16_t data_pair;
                switch (pairing_decoding_case) {
                    case 0:
                        data_pair = au.get(core::GenSub::PAIR_SAME_REC).pull();
                        read_1_first = !(((uint16_t)(data_pair)) & 1);
                        delta = ((uint16_t)(data_pair)) >> 1;
                        number_of_record_segments = 2;
                        break;
                    case 1:
                    case 3:
                    case 6:
                        number_of_record_segments = 1;
                        read_1_first = false;
                        if (combine_pairs) {
                            if (pairing_decoding_case == 1) {
                                mate_record_index_same_rec.push_back((uint32_t)(au.get(core::GenSub::PAIR_R1_SPLIT).pull()));
                                same_au_flag = true;
                            } else if (pairing_decoding_case == 3) {
                                mate_au_id.push_back((uint32_t)(au.get(core::GenSub::PAIR_R1_DIFF_SEQ).pull()));
                                mate_record_index.push_back((uint32_t)(au.get(core::GenSub::PAIR_R1_DIFF_POS).pull()));
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
            for (int i = 0; i < number_of_record_segments; i++) rlen[i] = (uint32_t)(au.get(core::GenSub::RLEN).pull()) + 1;  // rlen
            uint32_t pos = au.get(core::GenSub::POS_MAPPING_FIRST).pull();                                                                 // pos

            abs_pos += pos;
            std::string cur_read[2];
            cur_read[0] = refBuf.substr(abs_pos, rlen[0]);
            if (number_of_record_segments == 2) {
                uint64_t pair_abs_pos = abs_pos + delta;
                cur_read[1] = refBuf.substr(pair_abs_pos, rlen[1]);
            }
            bool reverseComp[2];
            for (int i = 0; i < number_of_record_segments; i++) reverseComp[i] = au.get(core::GenSub::RCOMP).pull();  // rcomp
            if (rtype == 3) {
                // type M
                for (int i = 0; i < number_of_record_segments; i++) {
                    uint32_t abs_mmpos = 0;
                    while (true) {
                        bool mmpos_flag = (bool)(au.get(core::GenSub::MMPOS_TERMINATOR).pull());
                        if (mmpos_flag == 1) break;
                        uint32_t mmpos = (uint32_t)(au.get(core::GenSub::MMPOS_POSITION).pull());
                        abs_mmpos += mmpos;
                        uint32_t mmtype_0 = (uint32_t)(au.get(core::GenSub::MMTYPE_TYPE).pull());
                        if (mmtype_0 != 0)  // i.e., not substitution
                            throw std::runtime_error("Non zero mmtype encountered.");
                        uint32_t mmtype_1 = (uint32_t)(au.get(core::GenSub::MMTYPE_SUBSTITUTION).pull());
                        cur_read[i][abs_mmpos] = int_to_char[mmtype_1];
                        abs_mmpos++;
                    }
                }
            }
            // finally, reverse complement if needed
            for (int i = 0; i < number_of_record_segments; i++) {
                if (reverseComp[i] == 1) cur_read[i] = reverse_complement(cur_read[i], rlen[i]);
            }
            bool first_read_flag = true;
            for (int i = 0; i < number_of_record_segments; i++) {
                if (paired_end) {
                    if ((i == 0 && read_1_first) || (i == 1 && !read_1_first)) {
                        first_read_flag = true;
                    } else {
                        first_read_flag = false;
                    }
                }
                cur_record.seq = cur_read[i];
                cur_record.name = names[name_pos];
                cur_record.qv = std::move(qvs[qv_pos++]);
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
            name_pos++;
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

}  // namespace spring
}  // namespace read
}  // namespace genie