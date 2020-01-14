#include "sam-importer.h"


namespace genie {
namespace sam {
    SamImporter::SamImporter(size_t _blockSize, std::istream &_file)
        : blockSize(_blockSize), samReader(_file), lock(), ref_counter(0) {}


        std::tuple<bool, uint8_t> SamImporter::convertFlags2Mpeg(uint16_t flags) {
        uint8_t flags_mpeg = 0;
        bool rcomp = false;
        flags_mpeg |= (flags & 1024u) >> 10u;  // PCR / duplicate
        flags_mpeg |= (flags & 512u) >> 8u;    // vendor quality check failed
        flags_mpeg |= (flags & 2u) << 1u;      // proper pair

        rcomp = flags & 16u;  // rcomp

        return std::make_tuple(rcomp, flags_mpeg);
    }

     char SamImporter::convertCigar2ECigarChar(char token) {
        static const auto lut_loc = []() -> std::string {
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
            UTILS_DIE("Invalid cigar token " + std::to_string(token));
        }
        char ret = lut_loc[token];
        if (ret == 0) {
            UTILS_DIE("Invalid cigar token" + std::to_string(token));
        }
        return ret;
    }

     int SamImporter::stepSequence(char token) {
        static const auto lut_loc = []() -> std::string {
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
            UTILS_DIE("Invalid cigar token " + std::to_string(token));
        }
        return lut_loc[token];
    }

     std::string SamImporter::convertCigar2ECigar(const std::string &cigar, const std::string &seq) {
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

     genie::mpegg_rec::MpeggRecord SamImporter::convert(uint16_t ref, sam::SamRecord &&_r1, sam::SamRecord *_r2) {
        sam::SamRecord r1 = std::move(_r1);
        auto flag_tuple = convertFlags2Mpeg(r1.getFlags());
        genie::mpegg_rec::MpeggRecord ret(_r2 ? 2 : 1, genie::mpegg_rec::ClassType::CLASS_I, r1.moveQname(), "Genie",
                                          std::get<1>(flag_tuple));

        genie::mpegg_rec::Alignment alignment(convertCigar2ECigar(r1.getCigar(), r1.getSeq()),
                                              r1.checkFlag(SamRecord::FlagPos::SEQ_REVERSE));
        alignment.addMappingScore(r1.getMapQ());
        genie::mpegg_rec::AlignmentContainer alignmentContainer(r1.getPos(), std::move(alignment));

        genie::mpegg_rec::Segment segment(r1.moveSeq());
        if (r1.getQual() != "*") {
            segment.addQualityValues(r1.moveQual());
        }
        ret.addRecordSegment(std::move(segment));

        if (_r2) {
            sam::SamRecord r2 = std::move(*_r2);

            auto ecigar = convertCigar2ECigar(r2.getCigar(), r2.getSeq());
            genie::mpegg_rec::Alignment alignment2(std::move(ecigar), r2.checkFlag(SamRecord::FlagPos::SEQ_REVERSE));
            alignment2.addMappingScore(r2.getMapQ());

            genie::mpegg_rec::Segment segment2(r2.moveSeq());
            if (r2.getQual() != "*") {
                segment2.addQualityValues(r2.moveQual());
            }
            ret.addRecordSegment(std::move(segment2));

            auto splitAlign = util::make_unique<genie::mpegg_rec::SplitAlignmentSameRec>(r2.getPos() - r1.getPos(),
                                                                                         std::move(alignment2));
            alignmentContainer.addSplitAlignment(std::move(splitAlign));
        } else {
        }

        ret.addAlignment(ref, std::move(alignmentContainer));  // TODO
        return ret;
    }

    bool SamImporter::pump(size_t id)  {
        genie::mpegg_rec::MpeggChunk chunk;
        std::vector<sam::SamRecord> s;
        std::list<sam::SamRecord> samRecords;
        uint16_t local_ref_num = 0;
        {
            util::OrderedSection section(&lock, id);
            samReader.read(blockSize, s);
            auto it = refs.find(s.front().getRname());
            if (it == refs.end()) {
                local_ref_num = ref_counter;
                refs.insert(std::make_pair(s.front().getRname(), ref_counter++));
            } else {
                local_ref_num = it->second;
            }
        }
        std::copy(s.begin(), s.end(), std::back_inserter(samRecords));

        std::cout << "Read " << samRecords.size() << " SAM record(s) for access unit " << id << std::endl;
        size_t skipped = 0;
        while (!samRecords.empty()) {
            sam::SamRecord samRecord = std::move(samRecords.front());
            samRecords.erase(samRecords.begin());
            // Search for mate
            const std::string &rnameSearchString =
                samRecord.getRnext() == "=" ? samRecord.getRname() : samRecord.getRnext();
            auto mate = samRecords.begin();
            mate = samRecords.end();  // Disable pairs for now TODO: implement
            if (samRecord.getPnext() == samRecord.getPos() && samRecord.getRname() == rnameSearchString) {
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
                if ((samRecord.getFlags() & (1u << uint16_t(SamRecord::FlagPos::SEGMENT_UNMAPPED))) ||
                    samRecord.getCigar() == "*" || samRecord.getPos() == 0 || samRecord.getRname() == "*") {
                    skipped++;
                } else {
                    chunk.emplace_back(convert(local_ref_num, std::move(samRecord), nullptr));
                }
            } else {
                // TODO: note the filtering of unaligned reads above. Move this to the encoder
                chunk.emplace_back(convert(local_ref_num, std::move(samRecord), &*mate));
                samRecords.erase(mate);
            }
        }
        if (skipped) {
            std::cerr << "Skipped " << skipped << " unmapped reads! Those are currently not supported." << std::endl;
        }
        if (!chunk.empty()) {
            flowOut(std::move(chunk), id);
        }

        // Break if less than blockSize records were read from the SAM file
        return !samReader.isEnd();
    }

    void SamImporter::dryIn()  { dryOut(); }
}  // namespace sam
}  // namespace genie