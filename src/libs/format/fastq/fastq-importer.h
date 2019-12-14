#ifndef GENIE_FASTQ_IMPORTER_H
#define GENIE_FASTQ_IMPORTER_H

#include <util/make_unique.h>
#include <util/source.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/alignment-container.h>
#include <util/exceptions.h>

class FastqImporter : public Source<std::unique_ptr<format::mpegg_rec::MpeggChunk>> {
private:
    static constexpr size_t NUM_FILES = 2;
    static constexpr size_t NUM_LINES = 4;
    size_t blockSize;
    std::array<std::istream*, NUM_FILES> file;
    size_t record_counter;
public:
    FastqImporter(size_t _blockSize, std::istream *_file_1);
    FastqImporter(size_t _blockSize, std::istream *_file_1, std::istream *_file_2);
    bool pump() override;
};


#endif //GENIE_FASTQ_IMPORTER_H
