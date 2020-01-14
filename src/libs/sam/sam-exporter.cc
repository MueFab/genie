
#include "sam-exporter.h"

#include <backbone/constants.h>
#include <backbone/format-exporter.h>
#include <backbone/mpegg_rec/alignment-container.h>
#include <backbone/mpegg_rec/external-alignment.h>
#include <backbone/mpegg_rec/meta-alignment.h>
#include <backbone/mpegg_rec/mpegg-record.h>
#include <backbone/mpegg_rec/segment.h>
#include <util/log.h>
#include <util/make_unique.h>
#include <util/ordered-lock.h>
#include <util/ordered-section.h>
#include <util/original-source.h>
#include <util/source.h>
#include <list>
#include "sam-reader.h"
#include "sam-record.h"
#include "sam-writer.h"

namespace genie {
namespace sam {

int SamExporter::stepRef(char token) {
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
    if (token < 0) {
        UTILS_DIE("Invalid cigar token" + std::to_string(token));
    }
    return lut_loc[token];
}

char SamExporter::convertECigar2CigarChar(char token) {
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
    if (token < 0) {
        UTILS_DIE("Invalid cigar token" + std::to_string(token));
    }
    char ret = lut_loc[token];
    if (ret == 0) {
        UTILS_DIE("Invalid cigar token" + std::to_string(token));
    }
    return ret;
}

uint64_t SamExporter::mappedLength(const std::string& cigar) {
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

/**
 * @brief Unpaired mode
 * @param _file_1 Output file
 */
SamExporter::SamExporter(sam::SamFileHeader&& header, std::ostream& _file_1) : writer(std::move(header), _file_1) {}

SamExporter::Stash SamExporter::stashFromMainAlignment(const genie::mpegg_rec::AlignmentContainer& alignment,
                                                       const genie::mpegg_rec::MpeggRecord& rec) {
    Stash ret;
    ret.position = alignment.getPosition();
    ret.seq = rec.getRecordSegments().front().getSequence();
    if (rec.getRecordSegments().front().getQualities().empty()) {
        ret.qual = "*";
    } else {
        ret.qual = rec.getRecordSegments().front().getQualities().front();
    }
    ret.cigar = eCigar2Cigar(alignment.getAlignment().getECigar());
    ret.mappingScore =
        alignment.getAlignment().getMappingScores().empty() ? 0 : alignment.getAlignment().getMappingScores().front();
    ret.rcomp = alignment.getAlignment().getRComp();
    ret.flags = mpeggFlagsToSamFlags(rec.getFlags(), ret.rcomp);

    return ret;
}

uint16_t SamExporter::mpeggFlagsToSamFlags(uint8_t mpeggflags, bool rcomp) {
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

std::string SamExporter::eCigar2Cigar(const std::string& ecigar) {
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
        if (genie::getAlphabetProperties(genie::AlphabetID::ACGTN).isIncluded(c)) {
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

SamExporter::Stash SamExporter::stashFromSplitAlignment(const genie::mpegg_rec::SplitAlignmentSameRec& split,
                                                        const genie::mpegg_rec::MpeggRecord& rec,
                                                        uint64_t master_position, size_t rec_count) {
    Stash ret;
    ret.position = master_position + split.getDelta();
    ret.seq = rec.getRecordSegments()[rec_count].getSequence();
    ret.qual = rec.getRecordSegments()[rec_count].getQualities().front();
    ret.cigar = eCigar2Cigar(split.getAlignment().getECigar());
    ret.mappingScore = split.getAlignment().getMappingScores().front();
    ret.rcomp = split.getAlignment().getRComp();
    ret.flags = mpeggFlagsToSamFlags(rec.getFlags(), ret.rcomp);
    ;
    return ret;
}

void SamExporter::generateRecords(bool primary, std::vector<Stash>& stash, const genie::mpegg_rec::MpeggRecord& rec,
                                  std::vector<sam::SamRecord>& out) {
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
        std::string name = rec.getReadName().empty() ? "*" : rec.getReadName();
        out.emplace_back(name, stash[i].flags, "ref" + std::to_string(rec.getMetaAlignment().getSeqID()),
                         stash[i].position, stash[i].mappingScore, stash[i].cigar, "=", stash[i + 1].position, tlen,
                         stash[i].seq, stash[i].qual);
    }
    if (stash.front().rcomp) {
        stash[i].flags |= 32;
    }
    std::string name = rec.getReadName().empty() ? "*" : rec.getReadName();
    out.emplace_back(name, stash[i].flags, "ref" + std::to_string(rec.getMetaAlignment().getSeqID()), stash[i].position,
                     stash[i].mappingScore, stash[i].cigar, "=", stash.front().position, -tlen, stash[i].seq,
                     stash[i].qual);
}

std::vector<sam::SamRecord> SamExporter::convert(genie::mpegg_rec::MpeggRecord&& rec) {
    std::vector<sam::SamRecord> ret;

    std::vector<Stash> stash;

    bool primaryAlignment = true;
    for (const auto& alignment : rec.getAlignments()) {
        stash.emplace_back(stashFromMainAlignment(alignment, rec));
        uint64_t master_position = alignment.getPosition();
        size_t rec_count = 1;
        for (const auto& split_alignment : alignment.getSplitAlignments()) {
            if (split_alignment->getType() != genie::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
                UTILS_DIE("Only same rec split alignments supported");
            }
            const auto& split = dynamic_cast<genie::mpegg_rec::SplitAlignmentSameRec&>(*split_alignment);
            stash.emplace_back(stashFromSplitAlignment(split, rec, master_position, rec_count));
            rec_count++;
        }
        generateRecords(primaryAlignment, stash, rec, ret);
        stash.clear();
        primaryAlignment = false;
    }
    return ret;
}

void SamExporter::flowIn(genie::mpegg_rec::MpeggChunk&& records, size_t id) {
    genie::mpegg_rec::MpeggChunk recs = std::move(records);
    util::OrderedSection section(&lock, id);
    for (auto& rec : recs) {
        auto sam_recs = convert(std::move(rec));
        for (auto& srec : sam_recs) {
            writer.write(std::move(srec));
        }
    }
}

/**
 * @brief Accepts end of file marker
 */
void SamExporter::dryIn() {}
}  // namespace sam
}  // namespace genie
