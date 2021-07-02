/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/transcoder.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_group.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_reader.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sorter.h"
#include "apps/genie/transcode-sam/utils.h"
#include "genie/core/record/alignment_split/other-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

bool save_mgrecs_by_rid(std::list<genie::core::record::Record>& recs,
                        std::map<int32_t, genie::util::BitWriter>& bitwriters) {
    for (auto& rec : recs) {
        auto ref_id = rec.getAlignmentSharedData().getSeqID();

        try {
            auto& bitwriter = bitwriters.at(ref_id);

            // TODO(Yeremia): Handle case where harddrive is full
            rec.write(bitwriter);
        } catch (std::out_of_range& r) {
            return false;
        }
    }
    recs.clear();
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

ErrorCode sam_to_mgrec_phase1(Config& options, int& nref) {
    std::list<std::ofstream> p1_writers;
    std::map<int32_t, genie::util::BitWriter> p1_bitwriters;
    int res;
    SamRecordGroup buffer;

    std::list<genie::core::record::Record> records;

    auto sam_reader = SamReader(options.inputFile);
    if (!sam_reader.isReady() || !sam_reader.isValid()) {
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
            buffer = SamRecordGroup();
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
        iter_map->second.flush();  /// flush bitwriter
        iter_writer->close();      /// close phase1 file

        iter_map++;
        iter_writer++;
    }

    return ErrorCode::success;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string gen_p2_tmp_fpath(Config& options, int rid, int ifile) {
    return options.tmp_dir_path + "/" + std::to_string(rid) + '.' + std::to_string(ifile) + PHASE2_TMP_EXT;
}

// ---------------------------------------------------------------------------------------------------------------------

ErrorCode sam_to_mgrec_phase2(Config& options, int& nref) {
    std::ofstream total_output(options.outputFile, std::ios::binary | std::ios::trunc);
    genie::util::BitWriter total_output_writer(&total_output);

    /// Process MPEG-G records of each RefID
    for (auto iref = 0; iref < nref; iref++) {
        auto n_tmp_files = 0;

        std::string fpath = options.tmp_dir_path + "/" + std::to_string(iref) + PHASE1_EXT;
        SubfileReader mgg_reader(fpath);

        std::vector<genie::core::record::Record> buffer;

        /// Split mgrec into multiple files and the records are sorted
        while (mgg_reader.good()) {
            /// Read MPEG-G records
            while (buffer.size() < PHASE2_BUFFER_SIZE) {
                if (!mgg_reader.readRecord()) {
                    break;
                }
                /// Store unmapped record to output file
                if (mgg_reader.getRecord().getAlignments().empty()) {
                    mgg_reader.writeRecord(total_output_writer);
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

            reader_with_smallest_pos->writeRecord(total_output_writer);

            /// Close the current tmp file if it contains no more record;
            if (!reader_with_smallest_pos->readRecord()) {
                tmp_file_readers.erase(reader_with_smallest_pos);
            }
        }

        /// Remove all temporary file belonging to current RID
        for (auto i_file = 0; i_file < n_tmp_files; i_file++) {
            std::remove(gen_p2_tmp_fpath(options, iref, i_file).c_str());
        }
    }

    total_output_writer.flush();
    total_output.flush();

    return ErrorCode::success;
}

// ---------------------------------------------------------------------------------------------------------------------

void clean_phase1_files(Config& options, int& nref) {
    for (auto iref = 0; iref < nref; iref++) {
        std::string fpath = options.tmp_dir_path + "/" + std::to_string(iref) + PHASE1_EXT;
        std::remove(fpath.c_str());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ErrorCode transcode_sam2mpg(Config& options) {
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

// ---------------------------------------------------------------------------------------------------------------------

char convertECigar2CigarChar(char token) {
    static const auto lut_loc = []() -> std::string {
        std::string lut(128, 0);
        lut['='] = 'M';
        lut['+'] = 'I';
        lut['-'] = 'D';
        lut['*'] = 'N';
        lut[')'] = 'S';
        lut[']'] = 'H';
        lut['%'] = 'N';
        lut['/'] = 'N';
        return lut;
    }();
    UTILS_DIE_IF(token < 0, "Invalid cigar token" + std::to_string(token));
    char ret = lut_loc[token];
    UTILS_DIE_IF(ret == 0, "Invalid cigar token" + std::to_string(token));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

int stepRef(char token) {
    static const auto lut_loc = []() -> std::string {
        std::string lut(128, 0);
        lut['M'] = 1;
        lut['='] = 1;
        lut['X'] = 1;
        lut['I'] = 0;
        lut['D'] = 1;
        lut['N'] = 1;
        lut['S'] = 0;
        lut['H'] = 0;
        lut['P'] = 0;
        return lut;
    }();
    UTILS_DIE_IF(token < 0, "Invalid cigar token" + std::to_string(token));
    return lut_loc[token];
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t mappedLength(const std::string& cigar) {
    if (cigar == "*") {
        return 0;
    }
    std::string digits;
    uint64_t length = 0;
    for (const auto& c : cigar) {
        if (std::isdigit(c)) {
            digits += c;
            continue;
        }
        length += std::stoi(digits) * stepRef(c);
        digits.clear();
    }
    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string eCigar2Cigar(const std::string& ecigar) {
    std::string cigar;
    cigar.reserve(ecigar.size());
    size_t mismatchCount = 0;
    std::string mismatchDigits;
    for (const auto& c : ecigar) {
        if ((c == '[') || (c == '(')) {
            continue;
        }
        if (std::isdigit(c)) {
            if (mismatchCount) {
                mismatchDigits += c;
            } else {
                cigar.push_back(c);
            }
            continue;
        }
        if (genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN).isIncluded(c)) {
            ++mismatchCount;
        } else {
            if (mismatchCount) {
                cigar += std::to_string(mismatchCount) + 'X';
                cigar += mismatchDigits;
                mismatchDigits.clear();
                mismatchCount = 0;
            }
            cigar.push_back(convertECigar2CigarChar(c));
        }
    }
    if (mismatchCount) {
        cigar += std::to_string(mismatchCount) + 'X';
        mismatchCount = 0;
    }
    return cigar;
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t computeSAMFlags(size_t s, size_t a, const genie::core::record::Record& record) {
    uint16_t flags = 0;
    if (record.getNumberOfTemplateSegments() > 1) {
        flags |= 0x1;
    }
    if (record.getFlags() & genie::core::GenConst::FLAGS_PROPER_PAIR_MASK) {
        flags |= 0x2;
    }
    // This read is unmapped
    if (record.getClassID() == genie::core::record::ClassType::CLASS_U ||
        ((record.getClassID() == genie::core::record::ClassType::CLASS_HM) &&
         ((s == 1 && record.isRead1First()) || (s == 0 && !record.isRead1First())))) {
        flags |= 0x4;
    }
    // Paired read is unmapped
    if (record.getClassID() == genie::core::record::ClassType::CLASS_U ||
        ((record.getClassID() == genie::core::record::ClassType::CLASS_HM) &&
         ((s == 0 && record.isRead1First()) || (s == 1 && !record.isRead1First())))) {
        flags |= 0x8;
    }
    // First or second read?
    if ((s == 0 && record.getSegments().size() == 2) || (record.getSegments().size() == 1 && record.isRead1First())) {
        flags |= 0x40;
    } else {
        flags |= 0x80;
    }
    // Secondary alignment
    if (a > 0) {
        flags |= 0x100;
    }
    if (record.getFlags() & genie::core::GenConst::FLAGS_QUALITY_FAIL_MASK) {
        flags |= 0x200;
    }
    if (record.getFlags() & genie::core::GenConst::FLAGS_PCR_DUPLICATE_MASK) {
        flags |= 0x400;
    }
    return flags;
}

// ---------------------------------------------------------------------------------------------------------------------

void processFirstMappedSegment(size_t s, size_t a, const genie::core::record::Record& record, std::string& rname,
                               std::string& pos, int64_t& tlen, std::string& mapping_qual, std::string& cigar,
                               uint16_t& flags) {
    // This read is mapped, process mapping
    rname = std::to_string(record.getAlignmentSharedData().getSeqID());
    if ((s == 0 && record.isRead1First()) || (s == 1 && !record.isRead1First()) || record.getSegments().size() == 1) {
        // First segment is in primary alignment
        pos = std::to_string(record.getAlignments()[a].getPosition() + 1);
        tlen -= record.getAlignments()[a].getPosition() + 1;
        mapping_qual = record.getAlignments()[a].getAlignment().getMappingScores().empty()
                           ? "0"
                           : std::to_string(record.getAlignments()[a].getAlignment().getMappingScores().front());
        cigar = eCigar2Cigar(record.getAlignments()[a].getAlignment().getECigar());
        if (record.getAlignments()[a].getAlignment().getRComp()) {
            flags |= 0x10;
        }
    } else {
        // First segment is mapped in split alignment
        const auto& split = dynamic_cast<const genie::core::record::alignment_split::SameRec&>(
            *record.getAlignments()[a].getAlignmentSplits().front().get());
        cigar = eCigar2Cigar(split.getAlignment().getECigar());
        pos = std::to_string(record.getAlignments()[a].getPosition() + split.getDelta() + 1);
        tlen -= record.getAlignments()[a].getPosition() + split.getDelta() + 1 + mappedLength(cigar);
        mapping_qual = split.getAlignment().getMappingScores().empty()
                           ? "0"
                           : std::to_string(split.getAlignment().getMappingScores().front());
        if (split.getAlignment().getRComp()) {
            flags |= 0x10;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void processSecondMappedSegment(size_t s, const genie::core::record::Record& record, int64_t& tlen,
                               uint16_t& flags, std::string& pnext, std::string& rnext) {
    // According to SAM standard, primary alignments only
    auto split_type = record.getClassID() == genie::core::record::ClassType::CLASS_HM
                      ? genie::core::record::AlignmentSplit::Type::UNPAIRED
                      : record.getAlignments()[0].getAlignmentSplits().front()->getType();
    if ((((s == 1 && record.isRead1First()) || (s == 0 && !record.isRead1First())) &&
         split_type == genie::core::record::AlignmentSplit::Type::SAME_REC) ||
        (split_type == genie::core::record::AlignmentSplit::Type::UNPAIRED)) {
        // Paired read is first read
        pnext = std::to_string(record.getAlignments()[0].getPosition() + 1);
        tlen += record.getAlignments()[0].getPosition() + 1;
        rnext = std::to_string(record.getAlignmentSharedData().getSeqID());
        if (record.getAlignments()[0].getAlignment().getRComp()) {
            flags |= 0x20;
        }
    } else {
        // Paired read is second read
        if (split_type == genie::core::record::AlignmentSplit::Type::SAME_REC) {
            const auto& split = dynamic_cast<const genie::core::record::alignment_split::SameRec&>(
                *record.getAlignments()[0].getAlignmentSplits().front().get());
            pnext = std::to_string(record.getAlignments()[0].getPosition() + split.getDelta() + 1);
            tlen += record.getAlignments()[0].getPosition() + split.getDelta() + 1 +
                    mappedLength(eCigar2Cigar(split.getAlignment().getECigar()));

            rnext = std::to_string(record.getAlignmentSharedData().getSeqID());
            if (split.getAlignment().getRComp()) {
                flags |= 0x20;
            }
        } else if (split_type == genie::core::record::AlignmentSplit::Type::OTHER_REC) {
            const auto& split = dynamic_cast<const genie::core::record::alignment_split::OtherRec&>(
                *record.getAlignments()[0].getAlignmentSplits().front().get());
            rnext = std::to_string(split.getNextSeq());
            pnext = std::to_string(split.getNextPos() + 1);
            tlen = 0;  // Not available without reading second record
        } else {
            tlen = 0;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ErrorCode transcode_mpg2sam(Config& options) {
    std::ifstream input_file(options.inputFile);
    std::ofstream output_file(options.outputFile);
    genie::util::BitReader reader(input_file);

    while (reader.isGood()) {
        genie::core::record::Record record(reader);
        // One line per segment and alignment
        for (size_t s = 0; s < record.getSegments().size(); ++s) {
            for (size_t a = 0; a < std::max(record.getAlignments().size(), size_t(1)); ++a) {
                std::string sam_record = record.getName() + "\t";
                uint16_t flags = computeSAMFlags(s, a, record);
                bool mapped = !(flags & 0x4);
                bool other_mapped = !(flags & 0x8);

                std::string rname = "*";
                std::string pos = "0";
                std::string mapping_qual = "0";
                std::string cigar = "*";
                std::string rnext = "*";
                std::string pnext = "0";
                int64_t tlen = 0;
                if (mapped) {
                    // First segment is mapped
                    processFirstMappedSegment(s, a, record, rname, pos, tlen, mapping_qual, cigar, flags);
                } else if (record.getClassID() == genie::core::record::ClassType::CLASS_HM) {
                    // According to SAM standard, HM-like records should have same position for the unmapped part, too
                    pos = std::to_string(record.getAlignments()[a].getPosition() + 1);
                    rname = std::to_string(record.getAlignmentSharedData().getSeqID());
                    if (a > 0) {
                        // No need to write unmapped records for secondary alignmentd
                        continue;
                    }
                }

                if (other_mapped && record.getNumberOfTemplateSegments() == 2) {
                    processSecondMappedSegment(s, record, tlen, flags, pnext, rnext);
                } else {
                    rnext = rname;
                    pnext = pos;
                    tlen = 0;
                }

                // Use "=" shorthand
                if (rnext == rname && rnext != "*") {
                    rnext = "=";
                }

                if (record.getClassID() == genie::core::record::ClassType::CLASS_HM ||
                    record.getClassID() == genie::core::record::ClassType::CLASS_U) {
                    tlen = 0;
                }

                sam_record += std::to_string(flags) + "\t";
                sam_record += rname + "\t";
                sam_record += pos + "\t";
                sam_record += mapping_qual + "\t";
                sam_record += cigar + "\t";
                sam_record += rnext + "\t";
                sam_record += pnext + "\t";
                sam_record += std::to_string(tlen) + "\t";
                sam_record += record.getSegments()[s].getSequence() + "\t";
                if (record.getSegments()[s].getQualities().empty()) {
                    sam_record += "*\n";
                } else {
                    sam_record += record.getSegments()[s].getQualities()[0] + "\n";
                }

                output_file.write(sam_record.c_str(), sam_record.length());
            }
        }
    }
    return ErrorCode::success;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
