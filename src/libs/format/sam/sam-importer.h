#ifndef GENIE_SAM_IMPORTER_H
#define GENIE_SAM_IMPORTER_H

#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>

#include <util/log.h>
#include <util/make_unique.h>
#include <util/original-source.h>
#include <util/source.h>

#include <algorithm>
#include <list>
#include "sam-file-reader.h"
#include "sam-record.h"

class SamImporter : public Source<std::unique_ptr<format::mpegg_rec::MpeggChunk>>, public OriginalSource {
   private:
    std::istream *file;
    size_t blockSize;
    format::sam::SamFileReader samFileReader;
    size_t record_counter;

    static bool convertFlags2Mpeg(uint16_t flags, uint8_t *flags_mpeg);

    static std::unique_ptr<format::mpegg_rec::MpeggRecord> convert(const format::sam::SamRecord &r1);

    std::unique_ptr<format::mpegg_rec::MpeggRecord> convert(const format::sam::SamRecord &r1,
                                                            const format::sam::SamRecord &r2);

    static void deflateEcigarElement(std::string *cigar, size_t count, char cur);

   public:
    SamImporter(size_t _blockSize, std::istream *_file);

    static std::string convertCigar2eCigar(const std::string &cigar);

    static std::string inflateCigar(const std::string &cigar);
    static std::string deflateCigar(const std::string &cigar);
    static std::string deflateEcigar(const std::string &cigar);

    bool pump() override;
};

#endif  // GENIE_SAM_IMPORTER_H
