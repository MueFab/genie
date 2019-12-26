#ifndef GENIE_MGRECS_EXPORTER_H
#define GENIE_MGRECS_EXPORTER_H

#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/segment.h>
#include <util/bitwriter.h>
#include <util/drain.h>
#include <util/make_unique.h>
#include <util/ordered-lock.h>

class MgrecsExporter : public Drain<std::unique_ptr<format::mpegg_rec::MpeggChunk>> {
    util::BitWriter writer;
    OrderedLock lock;

   public:
    explicit MgrecsExporter(std::ostream *_file_1);

    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) override;

    void dryIn() override;
};

#endif  // GENIE_MGRECS_EXPORTER_H
