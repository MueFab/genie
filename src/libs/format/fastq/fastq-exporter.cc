/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "fastq-exporter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace fastq {

// ---------------------------------------------------------------------------------------------------------------------

FastqExporter::FastqExporter(std::ostream *_file_1) : file{_file_1} {}

// ---------------------------------------------------------------------------------------------------------------------

FastqExporter::FastqExporter(std::ostream *_file_1, std::ostream *_file_2) : file{_file_1, _file_2} {}

// ---------------------------------------------------------------------------------------------------------------------

void FastqExporter::flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) {
    (void)id;
    for (auto &i : *t) {
        for (size_t j = 0; j < i->getNumberOfRecords(); ++j) {
            // ID
            constexpr const char *ID_TOKEN = "@";
            file[j]->write(ID_TOKEN, 1);
            file[j]->write(i->getReadName().c_str(), i->getReadName().length());
            file[j]->write("\n", 1);

            // Sequence
            file[j]->write(i->getRecordSegment(j)->getSequence()->c_str(),
                           i->getRecordSegment(j)->getSequence()->length());
            file[j]->write("\n", 1);

            // Reserved Line
            constexpr const char *RESERVED_TOKEN = "+";
            file[j]->write(RESERVED_TOKEN, 1);
            file[j]->write("\n", 1);

            // Qualities
            if (i->getRecordSegment(j)->getQvDepth()) {
                file[j]->write(i->getRecordSegment(j)->getQuality(0)->c_str(),
                               i->getRecordSegment(j)->getQuality(0)->length());
            } else {
                // Make up default quality values
                std::string qual(i->getRecordSegment(j)->getSequence()->length(), '#');
                file[j]->write(qual.c_str(), qual.length());
            }
            file[j]->write("\n", 1);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FastqExporter::dryIn() {}

}  // namespace fastq
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------