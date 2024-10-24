/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/exporter.h"

#include <genie/core/record/alignment_split/other-rec.h>
#include <genie/core/record/record.h>

#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include "genie/util/ordered-section.h"
#include "genie/util/stop-watch.h"
#include "sam/sam_to_mgrec/transcoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam {

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(std::string refFile, std::string output_file_path)
    : fasta_file_path(std::move(refFile)), outputFilePath(std::move(output_file_path)) {}

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

int64_t mappedLength(const std::string& cigar) {
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

std::string eCigar2Cigar(const std::string& ecigar) {
    std::string cigar;
    cigar.reserve(ecigar.size());
    size_t matchCount = 0;
    std::string number_buffer;
    for (const auto& c : ecigar) {
        if ((c == '[') || (c == '(')) {
            continue;
        }
        if (std::isdigit(c)) {
            number_buffer.push_back(c);
            continue;
        }
        if (genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN).isIncluded(c)) {
            matchCount += 1;
        } else {
            if (c == '=') {
                matchCount += std::stol(number_buffer);
                number_buffer.clear();
                continue;
            } else if (matchCount) {
                cigar += std::to_string(matchCount) + "M";
                matchCount = 0;
            }
            cigar += number_buffer;
            number_buffer.clear();
            cigar.push_back(convertECigar2CigarChar(c));
        }
    }
    if (matchCount) {
        cigar += std::to_string(matchCount) + "M";
        matchCount = 0;
    }
    return cigar;
}

// ---------------------------------------------------------------------------------------------------------------------

void processSecondMappedSegment(size_t s, const genie::core::record::Record& record, int64_t& tlen, uint16_t& flags,
                                std::string& pnext, std::string& rnext,
                                genieapp::transcode_sam::sam::sam_to_mgrec::RefInfo& refinfo) {
    // According to SAM standard, primary alignments only
    auto split_type = record.getClassID() == genie::core::record::ClassType::CLASS_HM
                          ? genie::core::record::AlignmentSplit::Type::UNPAIRED
                          : record.getAlignments()[0].getAlignmentSplits().front()->getType();
    if ((s == 1 && split_type == genie::core::record::AlignmentSplit::Type::SAME_REC) ||
        (split_type == genie::core::record::AlignmentSplit::Type::UNPAIRED)) {
        // Paired read is first read
        pnext = std::to_string(record.getAlignments()[0].getPosition() + 1);
        tlen += record.getAlignments()[0].getPosition() + 1;
        rnext = refinfo.isValid() ? refinfo.getMgr()->ID2Ref(record.getAlignmentSharedData().getSeqID())
                                  : std::to_string(record.getAlignmentSharedData().getSeqID());
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

            rnext = refinfo.isValid() ? refinfo.getMgr()->ID2Ref(record.getAlignmentSharedData().getSeqID())
                                      : std::to_string(record.getAlignmentSharedData().getSeqID());
            if (split.getAlignment().getRComp()) {
                flags |= 0x20;
            }
        } else if (split_type == genie::core::record::AlignmentSplit::Type::OTHER_REC) {
            const auto& split = dynamic_cast<const genie::core::record::alignment_split::OtherRec&>(
                *record.getAlignments()[0].getAlignmentSplits().front().get());
            rnext =
                refinfo.isValid() ? refinfo.getMgr()->ID2Ref(split.getNextSeq()) : std::to_string(split.getNextSeq());
            pnext = std::to_string(split.getNextPos() + 1);
            tlen = 0;  // Not available without reading second record
        } else {
            tlen = 0;
        }
    }
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
        ((record.getClassID() == genie::core::record::ClassType::CLASS_HM) && (s == 1))) {
        flags |= 0x4;
    }
    // Paired read is unmapped
    if (record.getClassID() == genie::core::record::ClassType::CLASS_U ||
        ((record.getClassID() == genie::core::record::ClassType::CLASS_HM) && (s == 0))) {
        flags |= 0x8;
    }
    // First or second read?
    if ((record.isRead1First() && s == 0) || (!record.isRead1First() && s == 1)) {
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
                               uint16_t& flags, genieapp::transcode_sam::sam::sam_to_mgrec::RefInfo& refinfo) {
    // This read is mapped, process mapping
    rname = refinfo.isValid() ? refinfo.getMgr()->ID2Ref(record.getAlignmentSharedData().getSeqID())
                              : std::to_string(record.getAlignmentSharedData().getSeqID());
    if (s == 0) {
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

void Exporter::skipIn(const util::Section& id) { util::OrderedSection sec(&lock, id); }

void Exporter::flowIn(core::record::Chunk&& t, const util::Section& id) {
    core::record::Chunk data = std::move(t);
    std::vector<genie::core::record::Record> records = data.getData();
    getStats().add(data.getStats());
    util::Watch watch;
    util::OrderedSection section(&lock, id);
    size_t size_seq = 0;
    size_t size_qual = 0;
    size_t size_name = 0;

    genieapp::transcode_sam::sam::sam_to_mgrec::RefInfo refinf(fasta_file_path);

    if (!output_set && outputFilePath.substr(0, 2) != "-.") {
        output_stream = std::ofstream(outputFilePath);
        output_file = &output_stream.value();
    }
    if (!output_set) {
        *output_file << "@HD\tVN:1.6" << std::endl;
        for (const auto& s : refinf.getMgr()->getSequences()) {
            *output_file << "@SQ\tSN:" << s << "\tLN:" << std::to_string(refinf.getMgr()->getLength(s)) << std::endl;
        }
        output_set = true;
    }

    for (auto& record : data.getData()) {
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
                    processFirstMappedSegment(s, a, record, rname, pos, tlen, mapping_qual, cigar, flags, refinf);
                } else if (record.getClassID() == genie::core::record::ClassType::CLASS_HM) {
                    // According to SAM standard, HM-like records should have same position for the unmapped part, too
                    pos = std::to_string(record.getAlignments()[a].getPosition() + 1);
                    rname = refinf.isValid() ? refinf.getMgr()->ID2Ref(record.getAlignmentSharedData().getSeqID())
                                             : std::to_string(record.getAlignmentSharedData().getSeqID());
                    if (a > 0) {
                        // No need to write unmapped records for secondary alignmentd
                        continue;
                    }
                }

                if (other_mapped && record.getNumberOfTemplateSegments() == 2) {
                    processSecondMappedSegment(s, record, tlen, flags, pnext, rnext, refinf);
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

    getStats().addInteger("size-sam-seq", size_seq);
    getStats().addInteger("size-sam-name", size_name);
    getStats().addInteger("size-sam-qual", size_qual);
    getStats().addInteger("size-sam-total", size_qual + size_name + size_seq);
    getStats().addDouble("time-sam-export", watch.check());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::sam

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
