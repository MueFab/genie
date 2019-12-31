#ifndef GENIE_SAM_EXPORTER_H
#define GENIE_SAM_EXPORTER_H

#include <coding/constants.h>
#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>
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

class SamExporter : public Drain<format::mpegg_rec::MpeggChunk> {
    OrderedLock lock;  //!< @brief Lock to ensure in order execution
    SamWriter writer;

   public:
    static int stepRef(char token) {
        static const auto lut = []() -> std::string {
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
            UTILS_DIE("Invalid cigar token");
        }
        return lut[token];
    }

    static char convertECigar2CigarChar(char token) {
        static const auto lut = []() -> std::string {
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
            UTILS_DIE("Invalid cigar token");
        }
        char ret = lut[token];
        if (ret == 0) {
            UTILS_DIE("Invalid cigar token");
        }
        return ret;
    }

    static uint64_t mappedLength(const std::string& cigar) {
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
    explicit SamExporter(format::sam::SamFileHeader&& header, std::ostream& _file_1)
        : writer(std::move(header), _file_1) {}

    struct Stash {
        uint16_t flags;
        uint64_t position;
        uint8_t mappingScore;
        std::string cigar;
        std::string seq;
        std::string qual;
        bool rcomp;
    };

    static Stash stashFromMainAlignment(const format::mpegg_rec::AlignmentContainer& alignment,
                                        const format::mpegg_rec::MpeggRecord& rec) {
        Stash ret;
        ret.position = alignment.getPosition();
        ret.seq = rec.getRecordSegments().front().getSequence();
        ret.qual = rec.getRecordSegments().front().getQualities().front();
        ret.cigar = eCigar2Cigar(alignment.getAlignment().getECigar());
        ret.mappingScore = alignment.getAlignment().getMappingScores().front();
        ret.rcomp = alignment.getAlignment().getRComp();
        ret.flags = mpeggFlagsToSamFlags(rec.getFlags(), ret.rcomp);

        return ret;
    }

    static uint16_t mpeggFlagsToSamFlags(uint8_t mpeggflags, bool rcomp) {
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

    static std::string eCigar2Cigar(const std::string& ecigar) {
        std::string cigar;
        cigar.reserve(ecigar.size());
        size_t mismatchCount = 0;
        for (const auto& c : ecigar) {
            if ((c == '[') || (c == '(')) {
                continue;
            }
            if (std::isdigit(c)) {
                cigar.push_back(c);
                continue;
            }
            if (getAlphabetProperties(AlphabetID::ACGTN).isIncluded(c)) {
                ++mismatchCount;
            } else {
                if (mismatchCount) {
                    cigar += std::to_string(mismatchCount) + 'X';
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

    static Stash stashFromSplitAlignment(const format::mpegg_rec::SplitAlignmentSameRec& split,
                                         const format::mpegg_rec::MpeggRecord& rec, uint64_t master_position,
                                         size_t rec_count) {
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

    static void generateRecords(bool primary, std::vector<Stash>& stash, const format::mpegg_rec::MpeggRecord& rec,
                                std::vector<format::sam::SamRecord>& out) {
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
            out.emplace_back(rec.getReadName(), stash[i].flags, "GenieRef", stash[i].position, stash[i].mappingScore,
                             stash[i].cigar, "=", stash[i + 1].position, tlen, stash[i].seq, stash[i].qual);
        }
        if (stash.front().rcomp) {
            stash[i].flags |= 32;
        }
        out.emplace_back(rec.getReadName(), stash[i].flags, "GenieRef", stash[i].position, stash[i].mappingScore,
                         stash[i].cigar, "=", stash.front().position, -tlen, stash[i].seq, stash[i].qual);
    }

    std::vector<format::sam::SamRecord> convert(format::mpegg_rec::MpeggRecord&& rec) {
        std::vector<format::sam::SamRecord> ret;

        std::vector<Stash> stash;

        bool primaryAlignment = true;
        for (const auto& alignment : rec.getAlignments()) {
            stash.emplace_back(stashFromMainAlignment(alignment, rec));
            uint64_t master_position = alignment.getPosition();
            size_t rec_count = 1;
            for (const auto& split_alignment : alignment.getSplitAlignments()) {
                if (split_alignment->getType() != format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
                    UTILS_DIE("Only same rec split alignments supported");
                }
                const auto& split = dynamic_cast<format::mpegg_rec::SplitAlignmentSameRec&>(*split_alignment);
                stash.emplace_back(stashFromSplitAlignment(split, rec, master_position, rec_count));
                rec_count++;
            }
            generateRecords(primaryAlignment, stash, rec, ret);
            stash.clear();
            primaryAlignment = false;
        }
        return ret;
    }

    void flowIn(format::mpegg_rec::MpeggChunk&& records, size_t id) override {
        format::mpegg_rec::MpeggChunk recs = std::move(records);
        OrderedSection section(&lock, id);
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
    void dryIn() override {}
};

#endif  // GENIE_SAM_EXPORTER_H
