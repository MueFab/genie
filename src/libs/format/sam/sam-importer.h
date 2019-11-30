#ifndef GENIE_SAM_IMPORTER_H
#define GENIE_SAM_IMPORTER_H

#include <format/mpegg_rec/mpegg-record.h>

#include <util/make_unique.h>
#include <util/source.h>

#include <list>
#include <util/log.h>
#include "sam-record.h"
#include "sam-file-reader.h"
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/segment.h>
#include <format/mpegg_rec/alignment-container.h>
#include <algorithm>

class SamImporter : public Source<std::unique_ptr<format::mpegg_rec::MpeggChunk>> {
private:
    std::istream *file;
    size_t blockSize;

    static std::unique_ptr<std::string> convertCigar2eCigar(const std::string &cigar);

    static bool convertFlags2Mpeg(uint16_t flags, uint8_t *flags_mpeg);

    static std::unique_ptr<format::mpegg_rec::MpeggRecord> convert(const format::sam::SamRecord &r1);

    std::unique_ptr<format::mpegg_rec::MpeggRecord>
    convert(const format::sam::SamRecord &r1, const format::sam::SamRecord &r2);

public:
    SamImporter(size_t _blockSize, std::istream *_file);

    void go() override;
};


#endif //GENIE_SAM_IMPORTER_H
