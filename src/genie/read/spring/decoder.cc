/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#define NOMINMAX

#include "decoder.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "params.h"
#include "util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

// ---------------------------------------------------------------------------------------------------------------------

void decode_streams(core::AccessUnit& au, bool paired_end, bool combine_pairs,
                    std::array<std::vector<Record>, 2>& matched_records,
                    std::array<std::vector<Record>, 2>& unmatched_records, std::vector<uint32_t>& mate_au_id,
                    std::vector<uint32_t>& mate_record_index, std::vector<std::string>& names,
                    std::vector<std::string>& qvs) {
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

    while (!au.get(core::GenSub::RTYPE).end()) {
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
                                mate_record_index_same_rec.push_back(
                                    (uint32_t)(au.get(core::GenSub::PAIR_R1_SPLIT).pull()));
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
            for (int i = 0; i < number_of_record_segments; i++)
                rlen[i] = (uint32_t)(au.get(core::GenSub::RLEN).pull()) + 1;  // rlen
            uint32_t pos = au.get(core::GenSub::POS_MAPPING_FIRST).pull();    // pos

            abs_pos += pos;
            std::string cur_read[2];
            cur_read[0] = refBuf.substr(abs_pos, rlen[0]);
            if (number_of_record_segments == 2) {
                uint64_t pair_abs_pos = abs_pos + delta;
                cur_read[1] = refBuf.substr(pair_abs_pos, rlen[1]);
            }
            bool reverseComp[2];
            for (int i = 0; i < number_of_record_segments; i++)
                reverseComp[i] = au.get(core::GenSub::RCOMP).pull();  // rcomp
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
                if (!names.empty()) {
                    cur_record.name = names[name_pos];
                }
                if (!qvs.empty()) {
                    cur_record.qv = std::move(qvs[qv_pos++]);
                }
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

// ---------------------------------------------------------------------------------------------------------------------

Decoder::Decoder(const std::string& working_dir, bool comb_p, bool paired_end) : combine_pairs(comb_p) {
    basedir = working_dir;

    while (true) {
        std::string random_str = "tmp." + spring::random_string(10);
        basedir = working_dir + "/" + random_str + '/';
        if (!ghc::filesystem::exists(basedir)) break;
    }
    UTILS_DIE_IF(!ghc::filesystem::create_directory(basedir), "Cannot create temporary directory.");
    std::cout << "Temporary directory: " << basedir << "\n";

    // Read compression params
    std::string compression_params_file = "cp.bin";
    std::string file_quality = "quality_1";
    std::string file_id = "id_1";
    cp.ureads_flag = false;
    cp.paired_end = paired_end;
    // temporary files for combine pairs case
    file_unmatched_fastq1 = basedir + "/unmatched_1.fastq";
    file_unmatched_fastq2 = basedir + "/unmatched_2.fastq";
    if (cp.paired_end) {
        if (combine_pairs && !cp.ureads_flag) {
            fout_unmatched1.open(file_unmatched_fastq1);
            fout_unmatched2.open(file_unmatched_fastq2);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::flowIn(genie::core::AccessUnit&& t, const util::Section& id) {
    core::record::Chunk chunk;
    genie::core::AccessUnit au = entropyCodeAU(std::move(t), true);
    util::Watch watch;
    std::array<std::vector<Record>, 2> matched_records;
    std::array<std::vector<Record>, 2> unmatched_records;
    std::vector<uint32_t> mate_au_id;
    std::vector<uint32_t> mate_record_index;

    std::vector<std::string> ecigars;
    while (!au.get(core::GenSub::RTYPE).end()) {
        if (au.get(core::GenSub::RTYPE).pull() != 5) {
            ecigars.emplace_back(std::to_string(au.get(core::GenSub::RLEN).pull() + 1) + "+");
            if (cp.paired_end) {
                if (au.get(core::GenSub::PAIR_DECODING_CASE).pull() == 0) {
                    ecigars.emplace_back(std::to_string(au.get(core::GenSub::RLEN).pull() + 1) + "+");
                }
            }
        } else {
            au.get(core::GenSub::RLEN).pull();
        }
    }
    au.get(core::GenSub::PAIR_DECODING_CASE).setPosition(0);
    au.get(core::GenSub::RTYPE).setPosition(0);
    au.get(core::GenSub::RLEN).setPosition(0);

    watch.pause();
    auto names = namecoder->process(au.get(core::GenDesc::RNAME));
    au.getStats().add(std::get<1>(names));
    auto qvs = qvcoder->process(au.getParameters().getQVConfig(core::record::ClassType::CLASS_U), ecigars,
                                au.get(core::GenDesc::QV));
    au.getStats().add(std::get<1>(qvs));
    watch.resume();

    decode_streams(au, cp.paired_end, combine_pairs, matched_records, unmatched_records, mate_au_id, mate_record_index,
                   std::get<0>(names), std::get<0>(qvs));

    for (size_t i = 0; i < matched_records[0].size(); ++i) {
        chunk.getData().emplace_back(cp.paired_end ? 2 : 1, core::record::ClassType::CLASS_U,
                                     std::move(matched_records[0][i].name), "", 0);
        core::record::Segment seg(std::move(matched_records[0][i].seq));
        if (!matched_records[0][i].qv.empty()) {
            seg.addQualities(std::move(matched_records[0][i].qv));
        }
        chunk.getData().back().addSegment(std::move(seg));
        if (cp.paired_end) {
            core::record::Segment seg2(std::move(matched_records[1][i].seq));
            if (!matched_records[1][i].qv.empty()) {
                seg2.addQualities(std::move(matched_records[1][i].qv));
            }
            chunk.getData().back().addSegment(std::move(seg2));
        }
    }
    {
        util::OrderedSection sec(&lock, id);
        if (cp.paired_end && combine_pairs) {
            for (int j = 0; j < 2; j++) {
                std::ostream& tmpout = (j == 0) ? fout_unmatched1 : fout_unmatched2;
                for (auto& fastqRecord : unmatched_records[j]) {
                    tmpout << fastqRecord.name << "\n";
                    tmpout << fastqRecord.seq << "\n";
                    tmpout << fastqRecord.qv << "\n";
                }
            }
            mate_au_id_concat.insert(mate_au_id_concat.end(), mate_au_id.begin(), mate_au_id.end());
            mate_record_index_concat.insert(mate_record_index_concat.end(), mate_record_index.begin(),
                                            mate_record_index.end());
        }
    }

    size_t chunk_size = chunk.getData().size();
    if (cp.paired_end) {
        chunk_size *= 2;
    }
    chunk.setStats(std::move(au.getStats()));
    chunk.getStats().addDouble("time-spring-decoder", watch.check());
    au.clear();
    flowOut(std::move(chunk), util::Section{id.start, chunk_size, true});
    if (id.length - chunk_size > 0) {
        skipOut(util::Section{id.start + chunk_size, id.length - chunk_size, true});
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::readRec(std::ifstream& i, Record& r) {
    UTILS_DIE_IF(!std::getline(i, r.name), "Error reading tmp file");
    UTILS_DIE_IF(!std::getline(i, r.seq), "Error reading tmp file");
    UTILS_DIE_IF(!std::getline(i, r.qv), "Error reading tmp file");
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::add(core::record::Chunk& chunk, core::record::Record&& r, uint64_t& pos) {
    const size_t CHUNK_SIZE = 100000;
    chunk.getData().push_back(std::move(r));
    if (chunk.getData().size() >= CHUNK_SIZE) {
        size_t size = chunk.getData().size() * 2;
        flowOut(std::move(chunk), {pos, size, true});
        pos += size;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::flushIn(uint64_t& pos) {
    core::record::Chunk chunk;

    // now reorder the remaining unmatched reads so that they are paired and then
    // write them to file.
    if (!cp.ureads_flag && cp.paired_end && combine_pairs) {
        std::cout << "Order unmatched decoded reads..." << std::endl;
        fout_unmatched1.close();
        fout_unmatched2.close();
        // first write the reads in file 1 as it is
        std::ifstream fin_unmatched1(file_unmatched_fastq1);

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
            std::vector<Record> records_bin(bin_size);
            Record tmpFastqRecord;
            for (uint32_t i = 0; i <= size_unmatched / bin_size; i++) {
                uint32_t num_records_bin = bin_size;
                if (i == size_unmatched / bin_size) num_records_bin = size_unmatched % bin_size;
                if (num_records_bin == 0) break;  // we are done
                std::ifstream fin_unmatched2(file_unmatched_fastq2);
                uint32_t bin_start = i * bin_size;
                for (uint32_t j = 0; j < size_unmatched; j++) {
                    if (reverse_index[j] >= bin_start && reverse_index[j] < bin_start + num_records_bin) {
                        readRec(fin_unmatched2, records_bin[reverse_index[j] - bin_start]);
                    } else {
                        readRec(fin_unmatched2, tmpFastqRecord);
                    }
                }
                fin_unmatched2.close();
                for (uint32_t j = 0; j < num_records_bin; j++) {
                    readRec(fin_unmatched1, tmpFastqRecord);

                    core::record::Record r(2, core::record::ClassType::CLASS_U, std::move(tmpFastqRecord.name), "", 0);
                    core::record::Segment s1(std::move(tmpFastqRecord.seq));
                    if (!tmpFastqRecord.qv.empty()) {
                        s1.addQualities(std::move(tmpFastqRecord.qv));
                    }

                    core::record::Segment s2(std::move(records_bin[j].seq));
                    if (!records_bin[j].qv.empty()) {
                        s2.addQualities(std::move(records_bin[j].qv));
                    }

                    r.addSegment(std::move(s1));
                    r.addSegment(std::move(s2));

                    add(chunk, std::move(r), pos);
                }
            }
        }
        ghc::filesystem::remove(file_unmatched_fastq1);
        ghc::filesystem::remove(file_unmatched_fastq2);
    }

    size_t size = chunk.getData().size() * 2;
    if (size) {
        flowOut(std::move(chunk), {pos, size, true});
        pos += size;
    }

    ghc::filesystem::remove(basedir);

    flushOut(pos);
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::skipIn(const util::Section& id) {
    { util::OrderedSection sec(&lock, id); }
    skipOut(id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------