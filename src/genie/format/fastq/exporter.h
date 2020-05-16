/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_EXPORTER_H
#define GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/record.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/drain.h>
#include <genie/util/make-unique.h>
#include <genie/util/ordered-lock.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fastq {

/**
 * @brief Module to export MPEG-G record to fastq files
 */
class Exporter : public util::Drain<core::record::Chunk> {
    std::vector<std::ostream *> file;       //!< @brief Support for paired output files
    util::OrderedLock lock;                 //!< @brief Lock to ensure in order execution
    genie::core::stats::FastqStats *stats;  //!< @brief Stats collector (null => don't collect)

   public:
    /**
     * @brief Unpaired mode
     * @param _file_1 Output file
     */
    explicit Exporter(std::ostream &_file_1, genie::core::stats::FastqStats *_stats = nullptr);

    /**
     * @brief Paired mode
     * @param _file_1 Output file #1
     * @param _file_2 Output file #2
     */
    Exporter(std::ostream &_file_1, std::ostream &_file_2, genie::core::stats::FastqStats *_stats = nullptr);

    /**
     * @brief Process one chunk of MPEGG records
     * @param records Input records
     * @param id Block identifier (for multithreading)
     */
    void flowIn(core::record::Chunk &&records, const util::Section &id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fastq
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
