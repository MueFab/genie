#ifndef GENIE_SAM_IMPORTER_H
#define GENIE_SAM_IMPORTER_H

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
namespace format {
namespace sam {
class SamImporter : public Source<format::mpegg_rec::MpeggChunk>, public OriginalSource {
   private:
    size_t blockSize;
    SamReader samReader;
    OrderedLock lock;  //!< @brief Lock to ensure in order execution

   public:
    SamImporter(size_t _blockSize, std::istream &_file) : blockSize(_blockSize), samReader(_file), lock() {}

    static std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags) {
        uint8_t flags_mpeg = 0;
        bool rcomp = false;
        flags_mpeg |= (flags & 1024u) >> 10u;  // PCR / duplicate
        flags_mpeg |= (flags & 512u) >> 8u;    // vendor quality check failed
        flags_mpeg |= (flags & 2u) << 1u;      // proper pair

        rcomp = flags & 16u;  // rcomp

        return std::make_tuple(rcomp, flags_mpeg);
    }

    static char convertCigar2ECigarChar(char token) {
        static const auto lut = []() -> std::string {
            std::string lut(128, 0);
            lut['M'] = '=';
            lut['='] = '=';
            lut['X'] = '=';
            lut['I'] = '+';
            lut['D'] = '-';
            lut['N'] = '*';
            lut['S'] = ')';
            lut['H'] = ']';
            lut['P'] = ']';
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

    static int stepSequence(char token) {
        static const auto lut = []() -> std::string {
            std::string lut(128, 0);
            lut['M'] = 1;
            lut['='] = 1;
            lut['X'] = 1;
            lut['I'] = 1;
            lut['D'] = 0;
            lut['N'] = 0;
            lut['S'] = 1;
            lut['H'] = 0;
            lut['P'] = 0;
            return lut;
        }();
        if (token < 0) {
            UTILS_DIE("Invalid cigar token");
        }
        return lut[token];
    }

    static std::string convertCigar2ECigar(const std::string &cigar, const std::string &seq) {
        std::string ecigar;
        const size_t EXPECTED_ELONGATION = 4;  // Additional braces for softclips + hardclips
        ecigar.reserve(cigar.length() + EXPECTED_ELONGATION);
        size_t seq_pos = 0;
        std::string digits;
        for (const auto &a : cigar) {
            if (std::isdigit(a)) {
                digits += a;
                continue;
            }
            if (a == 'X') {
                size_t end = std::stoi(digits) + seq_pos;
                if (end >= seq.length()) {
                    UTILS_DIE("CIGAR not valid for seq");
                }
                for (; seq_pos < end; ++seq_pos) {
                    ecigar += std::toupper(seq[seq_pos]);
                }
            } else {
                if (a == 'S') {
                    ecigar += '(';
                } else if (a == 'H') {
                    ecigar += '[';
                }
                char token = convertCigar2ECigarChar(a);
                seq_pos += stepSequence(a) * std::stoi(digits);
                ecigar += digits;
                ecigar += token;
            }
            digits.clear();
        }
        return ecigar;
    }

    static format::mpegg_rec::MpeggRecord convert(format::sam::SamRecord &&_r1, format::sam::SamRecord *_r2) {
        format::sam::SamRecord r1 = std::move(_r1);
        auto flag_tuple = convertFlags2Mpeg(r1.getFlags());
        format::mpegg_rec::MpeggRecord ret(_r2 ? 2 : 1, format::mpegg_rec::ClassType::CLASS_I, r1.moveQname(), "Genie",
                                           std::get<1>(flag_tuple));

        format::mpegg_rec::Alignment alignment(convertCigar2ECigar(r1.getCigar(), r1.getSeq()),
                                               r1.checkFlag(SamRecord::FlagPos::SEQ_REVERSE));
        alignment.addMappingScore(r1.getMapQ());
        format::mpegg_rec::AlignmentContainer alignmentContainer(r1.getPos(), std::move(alignment));

        format::mpegg_rec::Segment segment(r1.moveSeq());
        if(r1.getQual() != "*") {
            segment.addQualityValues(r1.moveQual());
        }
        ret.addRecordSegment(std::move(segment));

        if (_r2) {
            format::sam::SamRecord r2 = std::move(*_r2);

            auto ecigar = convertCigar2ECigar(r2.getCigar(), r2.getSeq());
            format::mpegg_rec::Alignment alignment2(std::move(ecigar), r2.checkFlag(SamRecord::FlagPos::SEQ_REVERSE));
            alignment2.addMappingScore(r2.getMapQ());

            format::mpegg_rec::Segment segment2(r2.moveSeq());
            if(r2.getQual() != "*") {
                segment2.addQualityValues(r2.moveQual());
            }
            ret.addRecordSegment(std::move(segment2));

            auto splitAlign = util::make_unique<format::mpegg_rec::SplitAlignmentSameRec>(r2.getPos() - r1.getPos(),
                                                                                          std::move(alignment2));
            alignmentContainer.addSplitAlignment(std::move(splitAlign));
        } else {
        }

        ret.addAlignment(0, std::move(alignmentContainer));  // TODO
        return ret;
    }

    bool pump(size_t id) override {
        format::mpegg_rec::MpeggChunk chunk;
        std::vector<format::sam::SamRecord> s;
        std::list<format::sam::SamRecord> samRecords;
        {
            OrderedSection section(&lock, id);
            samReader.read(blockSize, s);
        }
        std::copy( s.begin(), s.end(), std::back_inserter( samRecords ) );

        LOG_TRACE << "Read " << samRecords.size() << " SAM record(s)";
        while (!samRecords.empty()) {
            format::sam::SamRecord samRecord = std::move(samRecords.front());
            samRecords.erase(samRecords.begin());
            // Search for mate
            const std::string &rnameSearchString =
                samRecord.getRnext() == "=" ? samRecord.getRname() : samRecord.getRnext();
            auto mate = samRecords.begin();
            mate = samRecords.end();
            if(samRecord.getPnext() == samRecord.getPos() && samRecord.getRname() == rnameSearchString) {
                mate = samRecords.end();
            }
            for (; mate != samRecords.end(); ++mate) {
                if (mate->getRname() == rnameSearchString && mate->getPos() == samRecord.getPnext()) {
                    // LOG_TRACE << "Found mate";
                    break;
                }
            }
            if (mate == samRecords.end()) {
                // LOG_TRACE << "Did not find mate";
                chunk.emplace_back(convert(std::move(samRecord), nullptr));
            } else {
                chunk.emplace_back(convert(std::move(samRecord), &*mate));
                samRecords.erase(mate);
            }
        }

        if (!chunk.empty()) {
            flowOut(std::move(chunk), id);
        }

        // Break if less than blockSize records were read from the SAM file
        return !samReader.isEnd();
    }

    void dryIn() override { dryOut(); }
};
}  // namespace sam
}  // namespace format

#endif  // GENIE_SAM_IMPORTER_H