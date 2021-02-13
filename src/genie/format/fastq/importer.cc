/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/core/record/class-type.h>
#include <genie/util/ordered-section.h>
#include <genie/util/watch.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fastq {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream &_file_1) : blockSize(_blockSize), file_list{&_file_1} {}

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream &_file_1, std::istream &_file_2)
    : blockSize(_blockSize), file_list{&_file_1, &_file_2} {}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pumpRetrieve(core::Classifier *_classifier) {
    util::Watch watch;
    core::record::Chunk chunk;
    size_t size_seq = 0;
    size_t size_qual = 0;
    size_t size_name = 0;
    bool eof = false;
    {
        for (size_t cur_record = 0; cur_record < blockSize; ++cur_record) {
            auto data = readData(file_list);
            if (data.empty()) {
                eof = true;
                break;
            }
            auto record = buildRecord(data);
            for (const auto &seg : record.getSegments()) {
                size_seq += seg.getSequence().size();
                for (const auto &q : seg.getQualities()) {
                    size_qual += q.size();
                }
            }
            size_name += record.getName().size() * 2;
            chunk.getData().push_back(std::move(record));
        }
    }

    chunk.getStats().addInteger("size-fastq-seq", size_seq);
    chunk.getStats().addInteger("size-fastq-qual", size_qual);
    chunk.getStats().addInteger("size-fastq-name", size_name);
    chunk.getStats().addInteger("size-fastq-total", size_name + size_qual + size_seq);
    chunk.getStats().addDouble("time-fastq-import", watch.check());
    _classifier->add(std::move(chunk));
    return !eof;
}

// ---------------------------------------------------------------------------------------------------------------------

core::record::Record Importer::buildRecord(std::vector<std::array<std::string, LINES_PER_RECORD>> data) {
    auto ret = core::record::Record(uint8_t (data.size()), core::record::ClassType::CLASS_U,
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
