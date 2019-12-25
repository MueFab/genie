#ifndef GENIE_MGRECS_IMPORTER_H
#define GENIE_MGRECS_IMPORTER_H

#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>
#include <util/bitreader.h>
#include <util/exceptions.h>
#include <util/make_unique.h>
#include <util/original-source.h>
#include <util/source.h>

class MgrecsImporter : public Source<std::unique_ptr<format::mpegg_rec::MpeggChunk>>, public OriginalSource {
   private:
    size_t blockSize;
    util::BitReader reader;
    size_t record_counter;

   public:
    MgrecsImporter(size_t _blockSize, std::istream *_file_1);

    bool pump() override;
};

#endif  // GENIE_MGRECS_IMPORTER_H
