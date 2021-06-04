#include "transcoder.h"

#include <algorithm>
#include <fstream>

#include "sam_group.h"
#include "sam_reader.h"
#include "sorter.h"
#include <transcoder/utils.h>

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

bool save_mgrecs_by_rid(std::list<genie::core::record::Record>& recs,
                        std::map<int32_t, genie::util::BitWriter>& bitwriters) {
    for (auto& rec : recs) {
        auto ref_id = rec.getAlignmentSharedData().getSeqID();

        try {
            UTILS_DIE_IF(rec.getClassID() != genie::core::record::ClassType::CLASS_I, "Invalid Class");
            auto bitwriter = bitwriters.at(ref_id);

            // TODO: Handle case where harddrive is full
            rec.write(bitwriter);

        } catch (std::out_of_range) {
            return false;
        }
    }
    recs.clear();
    return true;
}

ErrorCode sam_to_mgrec_phase1(Config& options, int& nref) {
    std::list<std::ofstream> p1_writers;
    std::map<int32_t, genie::util::BitWriter> p1_bitwriters;
    int res;
    SamRecordGroup buffer;

    std::list<genie::core::record::Record> records;

    auto sam_reader = SamReader(options.sam_file_path);
    if (!sam_reader.isReady() or !sam_reader.isValid()) {
        return ErrorCode::failure;
    }

    nref = sam_reader.getNumRef();
    for (int32_t i = 0; i < nref; i++) {
        std::string fpath = options.tmp_dir_path + "/" + std::to_string(i) + PHASE1_EXT;

        p1_writers.emplace_back(fpath, std::ios::trunc | std::ios::binary);
        p1_bitwriters.emplace(i, &(p1_writers.back()));
    }

    auto sam_rec = SamRecord();
    if (sam_reader.readSamRecord(sam_rec) < 0) {
        return ErrorCode::failure;
    }

    auto curr_qname = sam_rec.getQname();
    buffer.addRecord(std::move(sam_rec));

    while ((res = sam_reader.readSamRecord(sam_rec)) >= 0) {
        /// Add to buffers as long as it has the same QNAMEs
        if (sam_rec.getQname() != curr_qname) {
            curr_qname = sam_rec.getQname();
            buffer.convert(records);

            if (!save_mgrecs_by_rid(records, p1_bitwriters)) {
                UTILS_DIE("Cannot save MPEG-G Records");
            }
        }

        buffer.addRecord(std::move(sam_rec));
    }
    if (res < -1) {
        return ErrorCode::failure;
    }

    buffer.convert(records);
    if (!save_mgrecs_by_rid(records, p1_bitwriters)) {
        UTILS_DIE("Cannot save MPEG-G Records");
    }

    auto iter_map = p1_bitwriters.begin();
    auto iter_writer = p1_writers.begin();
    while (iter_map != p1_bitwriters.end() && iter_writer != p1_writers.end()) {
        iter_map->second.flush(); /// flush bitwriter
        iter_writer->close(); /// close phase1 file

        iter_map++;
        iter_writer++;
    }

    return ErrorCode::success;
}

std::string gen_p2_tmp_fpath(Config& options, int rid, int ifile) {
    return options.tmp_dir_path + "/" + std::to_string(rid) + '.' + std::to_string(ifile) + PHASE2_TMP_EXT;
}

ErrorCode sam_to_mgrec_phase2(Config& options, int& nref) {

    /// Process MPEG-G records of each RefID
    for (auto iref = 0; iref < nref; iref++) {
        auto n_tmp_files = 0;

        std::string fpath = options.tmp_dir_path + "/" + std::to_string(iref) + PHASE1_EXT;
        SubfileReader mgg_reader(fpath);

        std::string tmp_fpath = options.tmp_dir_path + "/" + std::to_string(iref) + PHASE2_EXT;
        std::ofstream p2_writer(tmp_fpath, std::ios::binary | std::ios::trunc);
        genie::util::BitWriter p2_bitwriter(&p2_writer);

        std::vector<genie::core::record::Record> buffer;

        /// Split mgrec into multiple files and the records are sorted
        while (mgg_reader.good()) {
            /// Read MPEG-G records
            while (mgg_reader.readRecord() && buffer.size() < PHASE2_BUFFER_SIZE) {
                UTILS_DIE_IF(mgg_reader.getRecord().getClassID() != genie::core::record::ClassType::CLASS_I,
                             "Invalid Class found while reading records from phase 1 transcoding");
                //
                /// Store unmapped record to output file
                if (mgg_reader.getRecord().getAlignments().empty()) {
                    mgg_reader.writeRecord(p2_bitwriter);
                } else {
                    buffer.emplace_back(mgg_reader.moveRecord());
                }
            }

            /// Sort records in buffer by POS
            std::sort(buffer.begin(), buffer.end(), compare);

            std::ofstream tmp_writer(gen_p2_tmp_fpath(options, iref, n_tmp_files++),
                                     std::ios::trunc | std::ios::binary);
            genie::util::BitWriter tmp_bitwriter(&tmp_writer);

            for (auto& rec : buffer) {
                rec.write(tmp_bitwriter);
                tmp_bitwriter.flush();
            }

            buffer.clear();
            tmp_writer.close();
        }

        /// Open and cache the first record from each temporary file
        std::list<SubfileReader> tmp_file_readers;
        for (auto i_file = 0; i_file < n_tmp_files; i_file++) {
            tmp_file_readers.emplace_back(gen_p2_tmp_fpath(options, iref, i_file));

            /// Try to cache the first MPEG-G record, otherwise delete
            if (!tmp_file_readers.back().readRecord()) {
                tmp_file_readers.pop_back();
            }
        }

        while (!tmp_file_readers.empty()) {
            /// Find reader containing MPEG-G record with smallest POS value
            auto reader_with_smallest_pos = tmp_file_readers.begin();
            for (auto iter = std::next(tmp_file_readers.begin()); iter != tmp_file_readers.end(); iter++) {
                if (iter->getPos() < reader_with_smallest_pos->getPos()) {
                    reader_with_smallest_pos = iter;
                }
            }

            reader_with_smallest_pos->writeRecord(p2_bitwriter);

            /// Close the current tmp file if it contains no more record;
            if (!reader_with_smallest_pos->readRecord()) {
                tmp_file_readers.erase(reader_with_smallest_pos);
            }
        }

        /// Close Phase 2 transcoding output file
        p2_bitwriter.flush();
        p2_writer.close();

        /// Remove all temporary file belonging to current RID
        for (auto i_file = 0; i_file < n_tmp_files; i_file++) {
            std::remove(gen_p2_tmp_fpath(options, iref, i_file).c_str());
        }
    }

    return ErrorCode::success;
}

void clean_phase1_files(Config& options, int& nref){
    for (auto iref = 0; iref < nref; iref++) {
        auto n_tmp_files = 0;

        std::string fpath = options.tmp_dir_path + "/" + std::to_string(iref) + PHASE1_EXT;
        std::remove(fpath.c_str());
    }
}

ErrorCode transcode(Config& options) {
    int nref;
    ErrorCode status;

    if ((status = sam_to_mgrec_phase1(options, nref)) != ErrorCode::success) {
        return ErrorCode::failure;
    }

    if ((status = sam_to_mgrec_phase2(options, nref)) != ErrorCode::success) {
        return ErrorCode::failure;
    }

    clean_phase1_files(options, nref);

    return ErrorCode::success;
}
}
}
}

}