#include "sam-importer.h"
#include <util/ordered-section.h>

std::string SamImporter::convertCigar2eCigar(const std::string &cigar) {
    static const std::array<char, 256> lut = []() -> std::array<char, 256> {
        std::array<char, 256> ret{};
        ret['M'] = '=';
        ret['X'] = '=';
        ret['='] = '=';
        ret['I'] = '+';
        ret['D'] = '-';
        ret['N'] = '*';
        ret['S'] = ')';
        ret['H'] = ']';
        for (char c = '0'; c <= '9'; ++c) {
            ret[c] = c;
        }
        return ret;
    }();
    const size_t CLIPS_EXPECTED = 2;  // +2: Expect a grow of 2 from a clip on each end of the cigar

    std::string ret;
    ret.reserve(cigar.length() + CLIPS_EXPECTED);
    size_t lasttok = 0;
    for (size_t pos = 0; pos < cigar.length(); ++pos) {
        char transformed = lut[cigar[pos]];
        if (transformed == ')') {
            ret.insert(lasttok, 1, '(');
            lasttok = pos;
        } else if (transformed == ']') {
            ret.insert(lasttok, 1, '[');
            lasttok = pos;
        } else if (transformed == 0) {
            UTILS_DIE("Unknown CIGAR character");
        } else if (transformed < '0' || transformed > '9') {
            lasttok = pos;
        }
        ret.push_back(transformed);
    }

    // TODO: How to handle substitutions?

    return ret;
}

bool SamImporter::convertFlags2Mpeg(uint16_t flags, uint8_t *flags_mpeg) {
    *flags_mpeg = 0;
    *flags_mpeg |= (flags & 1024u) >> 10u;  // PCR / duplicate
    *flags_mpeg |= (flags & 512u) >> 8u;    // vendor quality check failed
    *flags_mpeg |= (flags & 2u) << 1u;      // proper pair

    return flags & 16u;  // rcomp
}

std::unique_ptr<format::mpegg_rec::MpeggRecord> SamImporter::convert(const format::sam::SamRecord &r1) {
    const uint8_t NUM_TEMPLATE_SEGMENTS = 1;

    uint8_t flags = 0;
    bool rcomp = convertFlags2Mpeg(r1.flag, &flags);
    auto ret = util::make_unique<format::mpegg_rec::MpeggRecord>(
        NUM_TEMPLATE_SEGMENTS, format::mpegg_rec::MpeggRecord::ClassType::CLASS_I,
        util::make_unique<std::string>(r1.qname), util::make_unique<std::string>("Genie"), flags);
    auto segment = util::make_unique<format::mpegg_rec::Segment>(util::make_unique<std::string>(r1.seq));
    if (!r1.qual.empty()) {
        segment->addQualityValues(util::make_unique<std::string>(r1.qual));
    }
    ret->addRecordSegment(std::move(segment));

    auto alignment = util::make_unique<format::mpegg_rec::Alignment>(
        util::make_unique<std::string>(convertCigar2eCigar(r1.cigar)), rcomp);
    alignment->addMappingScore(r1.mapq);
    auto alignmentContainer =
        util::make_unique<format::mpegg_rec::AlignmentContainer>(stoi(r1.rname), std::move(alignment));

    ret->addAlignment(stoi(r1.rname), std::move(alignmentContainer));

    return ret;
}

std::unique_ptr<format::mpegg_rec::MpeggRecord> SamImporter::convert(const format::sam::SamRecord &r1,
                                                                     const format::sam::SamRecord &r2) {
    const uint8_t NUM_TEMPLATE_SEGMENTS = 2;

    uint8_t flags = 0;
    bool rcomp = convertFlags2Mpeg(r1.flag, &flags);
    auto ret = util::make_unique<format::mpegg_rec::MpeggRecord>(
        NUM_TEMPLATE_SEGMENTS, format::mpegg_rec::MpeggRecord::ClassType::CLASS_I,
        util::make_unique<std::string>(r1.qname), util::make_unique<std::string>("Genie"), flags);

    // Sequences

    auto segment = util::make_unique<format::mpegg_rec::Segment>(util::make_unique<std::string>(r1.seq));
    if (!r1.qual.empty()) {
        segment->addQualityValues(util::make_unique<std::string>(r1.qual));
    }
    ret->addRecordSegment(std::move(segment));

    segment = util::make_unique<format::mpegg_rec::Segment>(util::make_unique<std::string>(r2.seq));
    if (!r2.qual.empty()) {
        segment->addQualityValues(util::make_unique<std::string>(r2.qual));
    }
    ret->addRecordSegment(std::move(segment));

    // Alignments

    auto alignment = util::make_unique<format::mpegg_rec::Alignment>(
        util::make_unique<std::string>(convertCigar2eCigar(r1.cigar)), rcomp);
    alignment->addMappingScore(r1.mapq);
    auto alignmentContainer =
        util::make_unique<format::mpegg_rec::AlignmentContainer>(stoi(r1.rname), std::move(alignment));

    alignment = util::make_unique<format::mpegg_rec::Alignment>(
        util::make_unique<std::string>(convertCigar2eCigar(r2.cigar)), rcomp);
    alignment->addMappingScore(r2.mapq);
    auto splitAlignment =
        util::make_unique<format::mpegg_rec::SplitAlignmentSameRec>(r2.pos - r1.pos, std::move(alignment));
    alignmentContainer->addSplitAlignment(std::move(splitAlignment));

    ret->addAlignment(stoi(r2.rname), std::move(alignmentContainer));

    return ret;
}

SamImporter::SamImporter(size_t _blockSize, std::istream *_file)
    : file(_file), blockSize(_blockSize), samFileReader(_file), record_counter(0) {}

bool SamImporter::pump(size_t id) {
    // Read a block of SAM records
    auto chunk = util::make_unique<format::mpegg_rec::MpeggChunk>();
    std::list<format::sam::SamRecord> samRecords;
    {
        OrderedSection sextion(&lock, id);
        samFileReader.readRecords(blockSize, &samRecords);
    }
    std::list<format::sam::SamRecord> samRecordsCopy(samRecords);
    LOG_TRACE << "Read " << samRecords.size() << " SAM record(s)";
    for (const auto &samRecord : samRecords) {
        // Search for mate
        std::string rnameSearchString;
        if (samRecord.rnext == "=") {
            rnameSearchString = samRecord.rname;
        } else {
            rnameSearchString = samRecord.rnext;
        }
        samRecordsCopy.erase(samRecordsCopy.begin());  // delete current record from the search space
        bool foundMate = false;
        for (auto it = samRecordsCopy.begin(); it != samRecordsCopy.end(); ++it) {
            if (it->rname == rnameSearchString && it->pos == samRecord.pnext) {
                // LOG_TRACE << "Found mate";

                chunk->push_back(convert(samRecord, *it));

                record_counter++;
                foundMate = true;
                samRecordsCopy.erase(it);
                break;
            }
        }
        if (!foundMate) {
            // LOG_TRACE << "Did not find mate";
            chunk->push_back(convert(samRecord));
            record_counter++;
        }

        // Break if everything was processed
        if (samRecordsCopy.empty()) {
            break;
        }
    }

    if (!chunk->empty()) {
        flowOut(std::move(chunk), record_counter);
    }

    // Break if less than blockSize records were read from the SAM file
    return samRecords.size() >= blockSize;
}

std::string SamImporter::inflateCigar(const std::string &cigar) {
    std::string ret;
    size_t count = 0;
    for (char cigar_pos : cigar) {
        if (cigar_pos == '(' || cigar_pos == '[') {  // Skip opening braces
            continue;
        }
        if (cigar_pos >= '0' && cigar_pos <= '9') {  // Process number
            count *= 10;
            count += cigar_pos - '0';
            continue;
        }

        char c = cigar_pos;
        if (count == 0) {
            count = 1;
            if (c == '-') {  // Distinguish deletions from character '-'
                c = '_';
            }
        }
        ret.insert(ret.size() - 1, count, c);
        count = 0;
    }
    return ret;
}

std::string SamImporter::deflateCigar(const std::string &cigar) {
    std::string ret;
    size_t count = 0;
    char cur = cigar.front();
    for (char cigar_pos : cigar) {
        if (cigar_pos == cur) {
            ++count;
            continue;
        }
        cur = cigar_pos;
        count = 0;
    }
    ret += std::to_string(count);
    ret += cur;
    return ret;
}

std::string SamImporter::deflateEcigar(const std::string &cigar) {
    std::string ret;
    size_t count = 0;
    char cur = cigar.front();
    for (char cigar_pos : cigar) {
        if (cigar_pos == cur) {
            ++count;
            continue;
        }
        deflateEcigarElement(&ret, count, cur);
        cur = cigar_pos;
        count = 0;
    }
    deflateEcigarElement(&ret, count, cur);
    return ret;
}

void SamImporter::deflateEcigarElement(std::string *cigar, size_t count, char cur) {
    static const std::array<bool, 256> lut = []() -> std::array<bool, 256> {
        std::array<bool, 256> ret{};
        ret['='] = true;
        ret['+'] = true;
        ret['-'] = true;
        ret[')'] = true;
        ret[']'] = true;
        ret['*'] = true;
        ret['/'] = true;
        ret['%'] = true;
        return ret;
    }();

    if (cur == ']') {
        *cigar += '[';
    } else if (cur == ')') {
        *cigar += '(';
    }
    if (lut[cur]) {
        *cigar += std::to_string(count);
    }
    if (cur == '_') {
        cur = '-';
    }
    *cigar += cur;
}
void SamImporter::dryIn() { dryOut(); }
