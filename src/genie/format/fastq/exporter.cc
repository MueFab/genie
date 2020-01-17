/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "exporter.h"
#include <genie/util/ordered-section.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fastq {

// ---------------------------------------------------------------------------------------------------------------------

FastqExporter::FastqExporter(std::ostream *_file_1) : file{_file_1} {}

// ---------------------------------------------------------------------------------------------------------------------

FastqExporter::FastqExporter(std::ostream *_file_1, std::ostream *_file_2) : file{_file_1, _file_2} {}

// ---------------------------------------------------------------------------------------------------------------------

void FastqExporter::flowIn(core::record::MpeggChunk &&t, size_t id) {
    core::record::MpeggChunk data = std::move(t);
    util::OrderedSection section(&lock, id);
    for (const auto &i : data) {
        auto file_ptr = *file.data();
        for (const auto &rec : i.getSegments()) {
            // ID
            constexpr const char *ID_TOKEN = "@";
            file_ptr->write(ID_TOKEN, 1);
            file_ptr->write(i.getName().c_str(), i.getName().length());
            file_ptr->write("\n", 1);

            // Sequence
            file_ptr->write(rec.getSequence().c_str(), rec.getSequence().length());
            file_ptr->write("\n", 1);

            // Reserved Line
            constexpr const char *RESERVED_TOKEN = "+";
            file_ptr->write(RESERVED_TOKEN, 1);
            file_ptr->write("\n", 1);

            // Qualities
            if (!rec.getQualities().empty()) {
                file_ptr->write(rec.getQualities().front().c_str(), rec.getQualities().front().length());
            } else {
                // Make up default quality values
                std::string qual(rec.getSequence().length(), '#');
                file_ptr->write(qual.c_str(), qual.length());
            }
            file_ptr->write("\n", 1);
            file_ptr++;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FastqExporter::dryIn() {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fastq
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------