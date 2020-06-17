/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_DECOMPRESS_H_
#define SPRING_DECOMPRESS_H_

#include <genie/core/read-decoder.h>
#include <genie/util/ordered-section.h>
#include <genie/util/watch.h>
#include <filesystem/filesystem.hpp>
#include <map>
#include <string>
#include <vector>
#include "params.h"
#include "util.h"

namespace genie {
namespace read {
namespace spring {

struct Record {
    std::string name;
    std::string seq;
    std::string qv;
};

void decode_streams(core::AccessUnit& au, bool paired_end, bool combine_pairs,
                    std::array<std::vector<Record>, 2>& matched_records,
                    std::array<std::vector<Record>, 2>& unmatched_records, std::vector<uint32_t>& mate_au_id,
                    std::vector<uint32_t>& mate_record_index, std::vector<std::string>& names,
                    std::vector<std::string>& qvs);

class Decoder : public genie::core::ReadDecoder {
   private:
    bool init;
    compression_params cp{};
    bool combine_pairs;
    size_t num_thr;
    util::OrderedLock lock;
    std::ofstream fout_unmatched1;
    std::ofstream fout_unmatched2;

    std::string file_unmatched_fastq1;
    std::string file_unmatched_fastq2;
    std::string basedir;

    std::vector<uint32_t> mate_au_id_concat, mate_record_index_concat;

   public:
    explicit Decoder(const std::string& working_dir, bool comb_p, bool paired_end, size_t threads)
        : combine_pairs(comb_p), num_thr(threads) {
        init = false;
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
    void flowIn(genie::core::AccessUnit&& t, const util::Section& id) override {
        core::record::Chunk chunk;
        genie::core::AccessUnit au = entropyCodeAU(std::move(t));
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

        decode_streams(au, cp.paired_end, combine_pairs, matched_records, unmatched_records, mate_au_id,
                       mate_record_index, std::get<0>(names), std::get<0>(qvs));

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
        flowOut(std::move(chunk), util::Section{id.start, chunk_size, true});
        if (id.length - chunk_size > 0) {
            skipOut(util::Section{id.start + chunk_size, id.length - chunk_size, true});
        }
    }

    static void readRec(std::ifstream& i, Record& r) {
        UTILS_DIE_IF(!std::getline(i, r.name), "Error reading tmp file");
        UTILS_DIE_IF(!std::getline(i, r.seq), "Error reading tmp file");
        UTILS_DIE_IF(!std::getline(i, r.qv), "Error reading tmp file");
    }

    void add(core::record::Chunk& chunk, core::record::Record&& r, size_t& pos) {
        const size_t CHUNK_SIZE = 100000;
        chunk.getData().push_back(std::move(r));
        if(chunk.getData().size() >= CHUNK_SIZE) {
            size_t size = chunk.getData().size() * 2;
            flowOut(std::move(chunk), {pos, size, true});
            pos += size;
        }
    }

    void flushIn(size_t& pos) override {
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

                        core::record::Record r(2, core::record::ClassType::CLASS_U, std::move(tmpFastqRecord.name), "",
                                               0);
                        core::record::Segment s1(std::move(tmpFastqRecord.seq));
                        s1.addQualities(std::move(tmpFastqRecord.qv));

                        core::record::Segment s2(std::move(records_bin[j].seq));
                        s2.addQualities(std::move(records_bin[j].qv));

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
        if(size) {
            flowOut(std::move(chunk), {pos, size, true});
            pos += size;
        }

        ghc::filesystem::remove(basedir);

        flushOut(pos);
    }

    void skipIn(const util::Section& id) override { {util::OrderedSection sec(&lock, id);} skipOut(id); }
};

}  // namespace spring
}  // namespace read
}  // namespace genie
#endif  // SPRING_DECOMPRESS_H_
