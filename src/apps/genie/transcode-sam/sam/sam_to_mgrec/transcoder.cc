/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/transcoder.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_group.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_reader.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sorter.h"
#include "apps/genie/transcode-sam/utils.h"
#include "boost/optional/optional.hpp"
#include "genie/core/record/alignment_split/other-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

void sam_to_mgrec_phase1(Config& options, int& chunk_id) {
    auto sam_reader = SamReader(options.inputFile);
    UTILS_DIE_IF(!sam_reader.isReady() || !sam_reader.isValid(), "Cannot open SAM file.");

    chunk_id = 0;
    while (true) {
        int ret = 0;
        std::vector<genie::core::record::Record> output_buffer;
        std::vector<std::vector<SamRecord>> queries;
        std::cerr << "Converting chunk " << chunk_id << "..." << std::endl;
        for (int i = 0; i < PHASE2_BUFFER_SIZE; ++i) {
            queries.emplace_back();
            ret = sam_reader.readSamQuery(queries.back());
            UTILS_DIE_IF(ret < -1, "Error reading sam query");
            if (ret == -1) {
                break;
            }
        }
        for (auto& q : queries) {
            SamRecordGroup buffer;
            for (auto& s : q) {
                buffer.addRecord(std::move(s));
            }
            std::list<genie::core::record::Record> records;
            buffer.convert(records);
            for (auto& m : records) {
                output_buffer.push_back(std::move(m));
            }
            q.clear();
        }
        queries.clear();

        std::cerr << "Sorting chunk " << chunk_id << "..." << std::endl;
        std::sort(output_buffer.begin(), output_buffer.end(), compare);
        std::cerr << "Writing chunk " << chunk_id << "..." << std::endl;
        {
            std::string fpath = options.tmp_dir_path + "/" + std::to_string(chunk_id) + PHASE1_EXT;
            std::ofstream output_file(fpath, std::ios::trunc | std::ios::binary);
            genie::util::BitWriter bwriter(&output_file);

            for (auto& r : output_buffer) {
                r.write(bwriter);
            }

            bwriter.flush();
        }

        chunk_id++;

        if (ret == -1) {
            return;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sam_to_mgrec_phase2(Config& options, int num_chunks) {
    std::unique_ptr<std::ostream> total_output;
    std::ostream* out_stream = &std::cout;
    if (options.outputFile.substr(0, 2) != "-.") {
        total_output = genie::util::make_unique<std::ofstream>(options.outputFile, std::ios::binary | std::ios::trunc);
        out_stream = total_output.get();
    }
    genie::util::BitWriter total_output_writer(out_stream);

    std::vector<std::unique_ptr<SubfileReader>> readers;
    readers.reserve(num_chunks);
    for (int i = 0; i < num_chunks; ++i) {
        readers.emplace_back(
            genie::util::make_unique<SubfileReader>(options.tmp_dir_path + "/" + std::to_string(i) + PHASE1_EXT));
    }

    while (true) {
        int smallest = -1;
        for (int i = 0; i < static_cast<int>(readers.size());) {
            if (!readers[i]->getRecord()) {
                auto file = readers[i]->getPath();
                readers.erase(readers.begin() + i);
                std::remove(file.c_str());
            } else {
                if (smallest == -1) {
                    smallest = i;
                } else {
                    if (compare(readers[i]->getRecord().get(), readers[smallest]->getRecord().get())) {
                        smallest = i;
                    }
                }
                i++;
            }
        }

        if (smallest == -1) {
            break;
        }

        auto rec = readers[smallest]->moveRecord();
        rec.write(total_output_writer);
    }

    total_output_writer.flush();
    out_stream->flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void transcode_sam2mpg(Config& options) {
    int nref;

    sam_to_mgrec_phase1(options, nref);
    sam_to_mgrec_phase2(options, nref);
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

void processSecondMappedSegment(size_t s, const genie::core::record::Record& record, int64_t& tlen, uint16_t& flags,
                                std::string& pnext, std::string& rnext) {
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

void transcode_mpg2sam(Config& options) {
    std::istream* input_file = &std::cin;
    std::ostream* output_file = &std::cout;
    boost::optional<std::ifstream> input_stream;
    boost::optional<std::ofstream> output_stream;

    if (options.inputFile.substr(0, 2) != "-.") {
        input_stream = std::ifstream(options.inputFile);
        input_file = &input_stream.get();
    }

    if (options.outputFile.substr(0, 2) != "-.") {
        output_stream = std::ofstream(options.outputFile);
        output_file = &output_stream.get();
    }

    genie::util::BitReader reader(*input_file);

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

                output_file->write(sam_record.c_str(), sam_record.length());
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
