#ifndef GENIE_FASTQ_EXPORTER_H
#define GENIE_FASTQ_EXPORTER_H

#include <util/drain.h>
#include <util/make_unique.h>

#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/segment.h>
#include <format/mpegg_rec/alignment-container.h>


class FastqExporter : public Drain<std::unique_ptr<format::mpegg_rec::MpeggChunk>> {
    static constexpr size_t NUM_FILES = 2;
    static constexpr size_t NUM_LINES = 4;
    std::array<std::ostream *, NUM_FILES> file;

public:
    explicit FastqExporter(std::ostream *_file_1);

    FastqExporter(std::ostream *_file_1, std::ostream *_file_2);

    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) override;

    void dryIn() override;

};


#endif //GENIE_FASTQ_EXPORTER_H
