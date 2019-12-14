#include "fastq-importer.h"


FastqImporter::FastqImporter(size_t _blockSize, std::istream *_file_1) : blockSize(_blockSize), file{_file_1, nullptr},
                                                                         record_counter(0) {

}

FastqImporter::FastqImporter(size_t _blockSize, std::istream *_file_1, std::istream *_file_2) : blockSize(_blockSize),
                                                                                                file{_file_1, _file_2},
                                                                                                record_counter(0) {

}

bool FastqImporter::pump() {
    auto chunk = util::make_unique<format::mpegg_rec::MpeggChunk>();
    bool noEnd = true;
    for (size_t i = 0; i < blockSize; ++i) {
        std::array<std::array<std::string, NUM_LINES>, NUM_FILES> data;
        for (size_t j = 0; j < 1 + bool(file[1]); ++j) {
            for (size_t k = 0; k < NUM_LINES; ++k) {
                noEnd = noEnd && std::getline(*(file[j]), data[j][k]);
                if (!noEnd) {
                    break;
                }
            }
            if (!noEnd) {
                break;
            }
            if (data[j][0][0] != '@') {
                UTILS_DIE("Invald fastq identifier");
            }
            if (data[j][2][0] != '+') {
                UTILS_DIE("Invald fastq line 3");
            }

            if (data[j][1].size() != data[j][3].size() && !data[j][3].empty()) {
                UTILS_DIE("Qual and Seq in fastq do not match in length");
            }
        }

        if (!noEnd) {
            break;
        }

        const size_t NUM_TEMPLATE_SEGMENTS = file[1] ? 2 : 1;
        auto ret = util::make_unique<format::mpegg_rec::MpeggRecord>(
                NUM_TEMPLATE_SEGMENTS,
                format::mpegg_rec::MpeggRecord::ClassType::CLASS_U,
                util::make_unique<std::string>(data[0][0].substr(1)),
                util::make_unique<std::string>("Genie"),
                0
        );
        for (size_t j = 0; j < 1 + bool(file[1]); ++j) {
            auto seg = util::make_unique<format::mpegg_rec::Segment>(util::make_unique<std::string>(data[j][1]));
            if (!data[j][3].empty()) {
                seg->addQualityValues(util::make_unique<std::string>(data[j][3]));
            }
            ret->addRecordSegment(std::move(seg));
        }
        chunk->push_back(std::move(ret));
    }
    flowOut(std::move(chunk), record_counter++);

    if (!noEnd) {
        dryOut();
    }
    return noEnd;
}