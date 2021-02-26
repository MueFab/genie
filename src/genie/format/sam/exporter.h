/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_EXPORTER_H_
#define SRC_GENIE_FORMAT_SAM_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/format-exporter.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/format/sam/header/header.h"
#include "genie/format/sam/writer.h"
#include "genie/util/ordered-lock.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

/**
 * @brief
 */
class Exporter : public core::FormatExporter {
    util::OrderedLock lock;  //!< @brief Lock to ensure in order execution
    Writer writer;  //!< @brief

 public:
    /**
     * @brief
     * @param token
     * @return
     */
    static int stepRef(char token);

    /**
     * @brief
     * @param token
     * @return
     */
    static char convertECigar2CigarChar(char token);

    /**
     * @brief
     * @param cigar
     * @return
     */
    static uint64_t mappedLength(const std::string& cigar);

    /**
     * @brief Unpaired mode
     * @param header
     * @param _file_1 Output file
     */
    explicit Exporter(header::Header&& header, std::ostream& _file_1);

    /**
     * @brief
     */
    struct Stash {
        uint16_t flags;  //!< @brief
        uint64_t position;  //!< @brief
        uint8_t mappingScore;  //!< @brief
        std::string cigar;  //!< @brief
        std::string seq;  //!< @brief
        std::string qual;  //!< @brief
        bool rcomp;  //!< @brief
    };

    /**
     * @brief
     * @param alignment
     * @param rec
     * @return
     */
    static Stash stashFromMainAlignment(const core::record::AlignmentBox& alignment, const core::record::Record& rec);

    /**
     * @brief
     * @param mpeggflags
     * @param rcomp
     * @return
     */
    static uint16_t mpeggFlagsToSamFlags(uint8_t mpeggflags, bool rcomp);

    /**
     * @brief
     * @param ecigar
     * @return
     */
    static std::string eCigar2Cigar(const std::string& ecigar);

    /**
     * @brief
     * @param split
     * @param rec
     * @param master_position
     * @param rec_count
     * @return
     */
    static Stash stashFromSplitAlignment(const core::record::alignment_split::SameRec& split,
                                         const core::record::Record& rec, uint64_t master_position, size_t rec_count);

    /**
     * @brief
     * @param primary
     * @param stash
     * @param rec
     * @param out
     */
    static void generateRecords(bool primary, std::vector<Stash>& stash, const core::record::Record& rec,
                                std::vector<sam::Record>& out);

    /**
     * @brief
     * @param rec
     * @return
     */
    std::vector<sam::Record> convert(core::record::Record&& rec);

    /**
     * @brief
     * @param records
     * @param id
     */
    void flowIn(core::record::Chunk&& records, const util::Section& id) override;

    /**
     * @brief
     * @param pos
     */
    void flushIn(uint64_t& pos) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
