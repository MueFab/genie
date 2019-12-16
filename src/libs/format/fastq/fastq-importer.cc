/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "fastq-importer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace fastq {

// ---------------------------------------------------------------------------------------------------------------------

FastqImporter::FastqImporter(size_t _blockSize, std::istream *_file_1)
    : blockSize(_blockSize), file_list{_file_1}, record_counter(0) {}

// ---------------------------------------------------------------------------------------------------------------------

FastqImporter::FastqImporter(size_t _blockSize, std::istream *_file_1, std::istream *_file_2)
    : blockSize(_blockSize), file_list{_file_1, _file_2}, record_counter(0) {}

// ---------------------------------------------------------------------------------------------------------------------

bool FastqImporter::pump() {
    auto chunk = util::make_unique<format::mpegg_rec::MpeggChunk>();
    bool eof = false;
    for (size_t cur_record = 0; cur_record < blockSize; ++cur_record) {
        auto data = readData(file_list);
        if (data.empty()) {
            eof = true;
            break;
        }
        chunk->push_back(buildRecord(data));
    }
    flowOut(std::move(chunk), record_counter++);

    if (eof) {
        dryOut();
    }
    return !eof;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<format::mpegg_rec::MpeggRecord> FastqImporter::buildRecord(
    const std::vector<std::array<std::string, LINES_PER_RECORD>> &data) {
    auto ret = util::make_unique<format::mpegg_rec::MpeggRecord>(
        data.size(), format::mpegg_rec::MpeggRecord::ClassType::CLASS_U,
        util::make_unique<std::string>(data[Files::FIRST][Lines::ID].substr(1)),
        util::make_unique<std::string>("Genie"), 0);

    for (const auto &cur_rec : data) {
        auto seg =
            util::make_unique<format::mpegg_rec::Segment>(util::make_unique<std::string>(cur_rec[Lines::SEQUENCE]));
        if (!cur_rec[Lines::QUALITY].empty()) {
            seg->addQualityValues(util::make_unique<std::string>(cur_rec[Lines::QUALITY]));
        }
        ret->addRecordSegment(std::move(seg));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::array<std::string, FastqImporter::LINES_PER_RECORD>> FastqImporter::readData(
    const std::vector<std::istream *> &_file_list) {
    std::vector<std::array<std::string, LINES_PER_RECORD>> data(_file_list.size());
    for (size_t cur_file = 0; cur_file < _file_list.size(); ++cur_file) {
        for (size_t cur_line = 0; cur_line < LINES_PER_RECORD; ++cur_line) {
            if (!std::getline(*(_file_list[cur_file]), data[cur_file][cur_line])) {
                data.clear();
                return data;
            }
        }

        sanityCheck(data[cur_file]);
    }
    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

void FastqImporter::sanityCheck(const std::array<std::string, LINES_PER_RECORD> &data) {
    constexpr char ID_TOKEN = '@';
    if (data[Lines::ID].front() != ID_TOKEN) {
        UTILS_DIE("Invald fastq identifier");
    }
    constexpr char RESERVED_TOKEN = '+';
    if (data[Lines::RESERVED].front() != RESERVED_TOKEN) {
        UTILS_DIE("Invald fastq line 3");
    }
    if (data[Lines::SEQUENCE].size() != data[Lines::QUALITY].size()) {
        UTILS_DIE("Qual and Seq in fastq do not match in length");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fastq
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------