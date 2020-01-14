#ifndef GENIE_SAM_EXPORTER_H
#define GENIE_SAM_EXPORTER_H

#include <backbone/constants.h>
#include <backbone/format-exporter.h>
#include <backbone/mpegg_rec/alignment-container.h>
#include <backbone/mpegg_rec/external-alignment.h>
#include <backbone/mpegg_rec/meta-alignment.h>
#include <backbone/mpegg_rec/mpegg-record.h>
#include <backbone/mpegg_rec/segment.h>
#include <util/log.h>
#include <util/make_unique.h>
#include <util/ordered-lock.h>
#include <util/ordered-section.h>
#include <util/original-source.h>
#include <util/source.h>
#include <list>
#include "sam-reader.h"
#include "sam-record.h"
#include "sam-writer.h"

namespace genie {
namespace sam {

class SamExporter : public genie::FormatExporter {
    util::OrderedLock lock;  //!< @brief Lock to ensure in order execution
    SamWriter writer;

   public:
    static int stepRef(char token);

    static char convertECigar2CigarChar(char token);

    static uint64_t mappedLength(const std::string& cigar);

    /**
     * @brief Unpaired mode
     * @param _file_1 Output file
     */
    explicit SamExporter(sam::SamFileHeader&& header, std::ostream& _file_1);

    struct Stash {
        uint16_t flags;
        uint64_t position;
        uint8_t mappingScore;
        std::string cigar;
        std::string seq;
        std::string qual;
        bool rcomp;
    };

    static Stash stashFromMainAlignment(const genie::mpegg_rec::AlignmentContainer& alignment,
                                        const genie::mpegg_rec::MpeggRecord& rec);

    static uint16_t mpeggFlagsToSamFlags(uint8_t mpeggflags, bool rcomp);

    static std::string eCigar2Cigar(const std::string& ecigar);

    static Stash stashFromSplitAlignment(const genie::mpegg_rec::SplitAlignmentSameRec& split,
                                         const genie::mpegg_rec::MpeggRecord& rec, uint64_t master_position,
                                         size_t rec_count);

    static void generateRecords(bool primary, std::vector<Stash>& stash, const genie::mpegg_rec::MpeggRecord& rec,
                                std::vector<sam::SamRecord>& out);

    std::vector<sam::SamRecord> convert(genie::mpegg_rec::MpeggRecord&& rec);

    void flowIn(genie::mpegg_rec::MpeggChunk&& records, size_t id) override;

    /**
     * @brief Accepts end of file marker
     */
    void dryIn() override;
};
}  // namespace sam
}  // namespace genie

#endif  // GENIE_SAM_EXPORTER_H
