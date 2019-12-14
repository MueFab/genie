#ifndef GENIE_SAM_EXPORTER_H
#define GENIE_SAM_EXPORTER_H

#include <util/drain.h>
#include <util/make_unique.h>

#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/segment.h>
#include <format/mpegg_rec/alignment-container.h>

class SamExporter : public Drain<std::unique_ptr<format::mpegg_rec::MpeggChunk>> {
    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) override;

    void dryIn() override;

    static std::string convertECigar2Cigar(const std::string &cigar);
};


#endif //GENIE_SAM_EXPORTER_H
