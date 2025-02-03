/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/fastq/exporter.h"
#include <string>
#include <utility>
#include "genie/util/ordered_section.h"
#include "genie/util/stop_watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fastq {

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(std::ostream &_file_1) : file{&_file_1} {}

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(std::ostream &_file_1, std::ostream &_file_2) : file{&_file_1, &_file_2} {}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::SkipIn(const Section &id) { util::OrderedSection sec(&lock, id); }

void Exporter::flowIn(core::record::Chunk &&t, const util::Section &id) {
    core::record::Chunk data = std::move(t);
    getStats().add(data.getStats());
    util::Watch watch;
    util::OrderedSection section(&lock, id);
    size_t size_seq = 0;
    size_t size_qual = 0;
    size_t size_name = 0;

    auto num_files = file.size();
    // ideally should be 2 for paired end, but we can handle 1 as well
    const char readname_suffix[2][3] = {"/1", "/2"};
    // suffix attached when paired end data but only one output fastq file
    bool second_read_flag = false;  // true when we are handling second read

    for (const auto &i : data.getData()) {
        auto file_ptr = file.data();
        second_read_flag = false;
        if (!i.isRead1First()) {
            second_read_flag = true;
            if (num_files == 2) file_ptr = &file.back();
        }
        for (const auto &rec : i.getSegments()) {
            // ID
            size_name += i.getName().size();
            constexpr const char *ID_TOKEN = "@";
            (*file_ptr)->write(ID_TOKEN, 1);
            (*file_ptr)->write(i.getName().c_str(), i.getName().length());
            if (i.getNumberOfTemplateSegments() == 2 && num_files == 1)
                (*file_ptr)->write(readname_suffix[second_read_flag], 2);
            (*file_ptr)->write("\n", 1);

            // Sequence
            size_seq += rec.getSequence().size();
            (*file_ptr)->write(rec.getSequence().c_str(), rec.getSequence().length());
            (*file_ptr)->write("\n", 1);

            // Reserved Line
            constexpr const char *RESERVED_TOKEN = "+";
            (*file_ptr)->write(RESERVED_TOKEN, 1);
            (*file_ptr)->write("\n", 1);

            // Qualities
            if (!rec.getQualities().empty()) {
                size_qual += rec.getQualities().front().size();
                (*file_ptr)->write(rec.getQualities().front().c_str(), rec.getQualities().front().length());
            } else {
                // Make up default quality values
                size_qual += rec.getSequence().length();
                std::string qual(rec.getSequence().length(), '#');
                (*file_ptr)->write(qual.c_str(), qual.length());
            }
            (*file_ptr)->write("\n", 1);
            second_read_flag = !second_read_flag;
            if (num_files == 2) {
                if (i.isRead1First()) {
                    file_ptr++;
                } else {
                    file_ptr--;
                }
            }
        }
    }

    getStats().addInteger("size-fastq-seq", size_seq);
    getStats().addInteger("size-fastq-name", size_name);
    getStats().addInteger("size-fastq-qual", size_qual);
    getStats().addInteger("size-fastq-total", size_qual + size_name + size_seq);
    getStats().addDouble("time-fastq-export", watch.check());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fastq
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
