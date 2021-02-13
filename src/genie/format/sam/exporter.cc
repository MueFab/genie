/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "exporter.h"
#include <genie/core/constants.h>
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/util/ordered-section.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

int Exporter::stepRef(char token) {
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

char Exporter::convertECigar2CigarChar(char token) {
    static const auto lut_loc = []() -> std::string {
        std::string lut(128, 0);
        lut['='] = '=';
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

uint64_t Exporter::mappedLength(const std::string& cigar) {
    std::string digits;
    uint64_t length = 0;
    for (const auto& c : cigar) {
        if (std::isdigit(c)) {
            digits += c;
            continue;
        }
        length += std::stoi(digits) * stepRef(c);
    }
    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(header::Header&& header, std::ostream& _file_1) : writer(std::move(header), _file_1) {}

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Stash Exporter::stashFromMainAlignment(const core::record::AlignmentBox& alignment,
                                                 const core::record::Record& rec) {
    Stash ret;
    ret.position = alignment.getPosition() + 1;
    ret.seq = rec.getSegments().front().getSequence();
    if (rec.getSegments().front().getQualities().empty()) {
        ret.qual = "*";
    } else {
        ret.qual = rec.getSegments().front().getQualities().front();
    }
    ret.cigar = eCigar2Cigar(alignment.getAlignment().getECigar());
    ret.mappingScore =
        alignment.getAlignment().getMappingScores().empty() ? 0 : alignment.getAlignment().getMappingScores().front();
    ret.rcomp = alignment.getAlignment().getRComp();
    ret.flags = mpeggFlagsToSamFlags(rec.getFlags(), ret.rcomp);

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Exporter::mpeggFlagsToSamFlags(uint8_t mpeggflags, bool rcomp) {
    uint16_t ret = 0;
    if (mpeggflags & 1u) {
        ret |= 1024u;
    }
    if (mpeggflags & 2u) {
        ret |= 512u;
    }
    if (mpeggflags & 4u) {
        ret |= 2u;
    }
    if (rcomp) {
        ret |= 16u;
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Exporter::eCigar2Cigar(const std::string& ecigar) {
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
        if (core::getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(c)) {
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

Exporter::Stash Exporter::stashFromSplitAlignment(const core::record::alignment_split::SameRec& split,
                                                  const core::record::Record& rec, uint64_t master_position,
                                                  size_t rec_count) {
    Stash ret;
    ret.position = master_position + split.getDelta();
    ret.seq = rec.getSegments()[rec_count].getSequence();
    ret.qual = rec.getSegments()[rec_count].getQualities().front();
    ret.cigar = eCigar2Cigar(split.getAlignment().getECigar());
    ret.mappingScore = split.getAlignment().getMappingScores().front();
    ret.rcomp = split.getAlignment().getRComp();
    ret.flags = mpeggFlagsToSamFlags(rec.getFlags(), ret.rcomp);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::generateRecords(bool primary, std::vector<Stash>& stash, const core::record::Record& rec,
                               std::vector<sam::Record>& out) {
    int64_t tlen =
        stash.size() > 1 ? stash.back().position + mappedLength(stash.back().cigar) - stash.front().position : 0;

    stash.front().flags |= 64;
    stash.back().flags |= 128;
    if (stash.size() > 1) {
        for (auto& a : stash) {
            a.flags |= 1;
        }
    }
    if (!primary) {
        for (auto& a : stash) {
            a.flags |= 2048;
        }
    }

    size_t i = 0;
    for (; i < stash.size() - 1; ++i) {
        if (stash[i + 1].rcomp) {
            stash[i].flags |= 32;
        }
        std::string name = rec.getName().empty() ? "*" : rec.getName();
        out.emplace_back(name, stash[i].flags, "ref" + std::to_string(rec.getAlignmentSharedData().getSeqID()),
                         (uint32_t) stash[i].position, stash[i].mappingScore, stash[i].cigar, "=", (uint32_t)stash[i + 1].position, (int32_t)tlen,
                         stash[i].seq, stash[i].qual);
    }
    if (stash.front().rcomp) {
        stash[i].flags |= 32;
    }
    std::string name = rec.getName().empty() ? "*" : rec.getName();
    out.emplace_back(name, stash[i].flags, "ref" + std::to_string(rec.getAlignmentSharedData().getSeqID()),
                     (uint32_t) stash[i].position, stash[i].mappingScore, stash[i].cigar, "=", (uint32_t)stash.front().position, (int32_t)-tlen,
                     stash[i].seq, stash[i].qual);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<sam::Record> Exporter::convert(core::record::Record&& rec) {
    std::vector<sam::Record> ret;

    std::vector<Stash> stash;

    bool primaryAlignment = true;
    for (const auto& alignment : rec.getAlignments()) {
        stash.emplace_back(stashFromMainAlignment(alignment, rec));
        uint64_t master_position = alignment.getPosition() + 1;
        size_t rec_count = 1;
        for (const auto& split_alignment : alignment.getAlignmentSplits()) {
            UTILS_DIE_IF(split_alignment->getType() != core::record::AlignmentSplit::Type::SAME_REC,
                         "Only same rec split alignments supported");
            const auto& split = dynamic_cast<core::record::alignment_split::SameRec&>(*split_alignment);
            stash.emplace_back(stashFromSplitAlignment(split, rec, master_position, rec_count));
            rec_count++;
        }
        generateRecords(primaryAlignment, stash, rec, ret);
        stash.clear();
        primaryAlignment = false;
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::flowIn(core::record::Chunk&& records, const util::Section& id) {
    core::record::Chunk recs = std::move(records);
    util::OrderedSection section(&lock, id);
    for (auto& rec : recs.getData()) {
        auto sam_recs = convert(std::move(rec));
        for (auto& srec : sam_recs) {
            writer.write(std::move(srec));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::flushIn(uint64_t&) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------