/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/core/record/class-type.h>
#include <genie/util/ordered-section.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fastq {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream &_file_1)
    : blockSize(_blockSize), file_list{&_file_1}, record_counter(0) {}

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream &_file_1, std::istream &_file_2)
    : blockSize(_blockSize), file_list{&_file_1, &_file_2}, record_counter(0) {}

// ---------------------------------------------------------------------------------------------------------------------

/*bool Importer::pump(size_t &) {
    core::record::Chunk chunk;
    bool eof = false;
    {
        //  util::OrderedSection section(&lock, id);
        for (size_t cur_record = 0; cur_record < blockSize; ++cur_record) {
            auto data = readData(file_list);
            if (data.empty()) {
                eof = true;
                break;
            }
            chunk.push_back(buildRecord(data));
        }
    }
    //   flowOut(std::move(chunk), record_counter++);

    return !eof;
}*/

// ---------------------------------------------------------------------------------------------------------------------

core::record::Record Importer::buildRecord(std::vector<std::array<std::string, LINES_PER_RECORD>> data) {
    auto ret = core::record::Record(data.size(), core::record::ClassType::CLASS_U,
                                    data[Files::FIRST][Lines::ID].substr(1), "Genie", 0);

    for (auto &cur_rec : data) {
        auto seg = core::record::Segment(std::move(cur_rec[Lines::SEQUENCE]));
        if (!cur_rec[Lines::QUALITY].empty()) {
            seg.addQualities(std::move(cur_rec[Lines::QUALITY]));
        }
        ret.addSegment(std::move(seg));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::array<std::string, Importer::LINES_PER_RECORD>> Importer::readData(
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

void Importer::sanityCheck(const std::array<std::string, LINES_PER_RECORD> &data) {
    constexpr char ID_TOKEN = '@';
    UTILS_DIE_IF(data[Lines::ID].front() != ID_TOKEN, "Invald fastq identifier");
    constexpr char RESERVED_TOKEN = '+';
    UTILS_DIE_IF(data[Lines::RESERVED].front() != RESERVED_TOKEN, "Invald fastq line 3");
    UTILS_DIE_IF(data[Lines::SEQUENCE].size() != data[Lines::QUALITY].size(),
                 "Qual and Seq in fastq do not match in length");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fastq
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
