/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SAM_EXPORTER_H
#define GENIE_SAM_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/format-exporter.h>
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/ordered-section.h>
#include "header/header.h"
#include "writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

/**
 *
 */
class Exporter : public core::FormatExporter {
    util::OrderedLock lock;  //!< @brief Lock to ensure in order execution
    Writer writer;           //!<

   public:
    /**
     *
     * @param token
     * @return
     */
    static int stepRef(char token);

    /**
     *
     * @param token
     * @return
     */
    static char convertECigar2CigarChar(char token);

    /**
     *
     * @param cigar
     * @return
     */
    static uint64_t mappedLength(const std::string& cigar);

    /**
     * @brief Unpaired mode
     * @param _file_1 Output file
     */
    explicit Exporter(header::Header&& header, std::ostream& _file_1);

    /**
     *
     */
    struct Stash {
        uint16_t flags;        //!<
        uint64_t position;     //!<
        uint8_t mappingScore;  //!<
        std::string cigar;     //!<
        std::string seq;       //!<
        std::string qual;      //!<
        bool rcomp;            //!<
    };

    /**
     *
     * @param alignment
     * @param rec
     * @return
     */
    static Stash stashFromMainAlignment(const core::record::AlignmentBox& alignment, const core::record::Record& rec);

    /**
     *
     * @param mpeggflags
     * @param rcomp
     * @return
     */
    static uint16_t mpeggFlagsToSamFlags(uint8_t mpeggflags, bool rcomp);

    /**
     *
     * @param ecigar
     * @return
     */
    static std::string eCigar2Cigar(const std::string& ecigar);

    /**
     *
     * @param split
     * @param rec
     * @param master_position
     * @param rec_count
     * @return
     */
    static Stash stashFromSplitAlignment(const core::record::alignment_split::SameRec& split,
                                         const core::record::Record& rec, uint64_t master_position, size_t rec_count);

    /**
     *
     * @param primary
     * @param stash
     * @param rec
     * @param out
     */
    static void generateRecords(bool primary, std::vector<Stash>& stash, const core::record::Record& rec,
                                std::vector<sam::Record>& out);

    /**
     *
     * @param rec
     * @return
     */
    std::vector<sam::Record> convert(core::record::Record&& rec);

    /**
     *
     * @param records
     * @param id
     */
    void flowIn(core::record::Chunk&& records, const util::Section& id) override;

    /**
     *
     * @param id
     */
    void skipIn(const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
