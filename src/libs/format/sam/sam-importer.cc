#include "sam-importer.h"


std::unique_ptr<std::string> SamImporter::convertCigar2eCigar(const std::string &cigar) {
    auto ret = util::make_unique<std::string>(cigar);
    std::replace(ret->begin(), ret->end(), 'M', '=');
    std::replace(ret->begin(), ret->end(), 'I', '+');
    std::replace(ret->begin(), ret->end(), 'D', '-');
    std::replace(ret->begin(), ret->end(), 'N', '*');
    // TODO: Handle Clips and Substitution
    return ret;
}

bool SamImporter::convertFlags2Mpeg(uint16_t flags, uint8_t *flags_mpeg) {
    *flags_mpeg = 0;
    *flags_mpeg |= (flags & 1024u) >> 10u; // PCR / duplicate
    *flags_mpeg |= (flags & 512u) >> 8u; // vendor quality check failed
    *flags_mpeg |= (flags & 2u) << 1u; // proper pair

    return flags & 16u;  // rcomp
}

std::unique_ptr<format::mpegg_rec::MpeggRecord> SamImporter::convert(const format::sam::SamRecord &r1) {
    const uint8_t NUM_TEMPLATE_SEGMENTS = 1;

    uint8_t flags = 0;
    bool rcomp = convertFlags2Mpeg(r1.flag, &flags);
    auto ret = util::make_unique<format::mpegg_rec::MpeggRecord>(
            NUM_TEMPLATE_SEGMENTS,
            format::mpegg_rec::MpeggRecord::ClassType::CLASS_I,
            util::make_unique<std::string>(r1.qname),
            util::make_unique<std::string>("Genie"),
            flags
    );
    auto segment = util::make_unique<format::mpegg_rec::Segment>(util::make_unique<std::string>(r1.seq));
    if (!r1.qual.empty()) {
        segment->addQualityValues(util::make_unique<std::string>(r1.qual));
    }
    ret->addRecordSegment(std::move(segment));

    auto alignment = util::make_unique<format::mpegg_rec::Alignment>(convertCigar2eCigar(r1.cigar), rcomp);
    alignment->addMappingScore(r1.mapq);
    auto alignmentContainer = util::make_unique<format::mpegg_rec::AlignmentContainer>(stoi(r1.rname),
                                                                                       std::move(alignment));

    ret->addAlignment(stoi(r1.rname), std::move(alignmentContainer));

    return ret;
}

std::unique_ptr<format::mpegg_rec::MpeggRecord>
SamImporter::convert(const format::sam::SamRecord &r1, const format::sam::SamRecord &r2) {
    const uint8_t NUM_TEMPLATE_SEGMENTS = 2;

    uint8_t flags = 0;
    bool rcomp = convertFlags2Mpeg(r1.flag, &flags);
    auto ret = util::make_unique<format::mpegg_rec::MpeggRecord>(
            NUM_TEMPLATE_SEGMENTS,
            format::mpegg_rec::MpeggRecord::ClassType::CLASS_I,
            util::make_unique<std::string>(r1.qname),
            util::make_unique<std::string>("Genie"),
            flags
    );

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

    auto alignment = util::make_unique<format::mpegg_rec::Alignment>(convertCigar2eCigar(r1.cigar), rcomp);
    alignment->addMappingScore(r1.mapq);
    auto alignmentContainer = util::make_unique<format::mpegg_rec::AlignmentContainer>(stoi(r1.rname),
                                                                                       std::move(alignment));

    alignment = util::make_unique<format::mpegg_rec::Alignment>(convertCigar2eCigar(r2.cigar), rcomp);
    alignment->addMappingScore(r2.mapq);
    auto splitAlignment = util::make_unique<format::mpegg_rec::SplitAlignmentSameRec>(r2.pos - r1.pos,
                                                                                      std::move(alignment));
    alignmentContainer->addSplitAlignment(std::move(splitAlignment));

    ret->addAlignment(stoi(r2.rname), std::move(alignmentContainer));

    return ret;
}

SamImporter::SamImporter(size_t _blockSize, std::istream *_file) : file(_file), blockSize(_blockSize) {
}

void SamImporter::go() {
    format::sam::SamFileReader samFileReader(file);
    uint32_t record_counter = 0;
    while (true) {
        // Read a block of SAM records
        auto chunk = util::make_unique<format::mpegg_rec::MpeggChunk>();
        std::list<format::sam::SamRecord> samRecords;
        samFileReader.readRecords(blockSize, &samRecords);
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
        if (samRecords.size() < blockSize) {
            break;
        }
    }
    dryOut();
}