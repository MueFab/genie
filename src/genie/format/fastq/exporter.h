/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTQ_EXPORTER_H_
#define SRC_GENIE_FORMAT_FASTQ_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/format-exporter.h"
#include "genie/core/record/alignment/chunk.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/drain.h"
#include "genie/util/make-unique.h"
#include "genie/util/ordered-lock.h"
#include "genie/util/ordered-section.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fastq {

/**
 * @brief Module to export MPEG-G record to fastq files
 */
class Exporter : public core::FormatExporter {
    std::vector<std::ostream *> file;  //!< @brief Support for paired output files
    util::OrderedLock lock;            //!< @brief Lock to ensure in order execution

 public:
    /**
     * @brief Unpaired mode
     * @param _file_1 Output file
     */
    explicit Exporter(std::ostream &_file_1);

    /**
     * @brief Paired mode
     * @param _file_1 Output file #1
     * @param _file_2 Output file #2
     */
    Exporter(std::ostream &_file_1, std::ostream &_file_2);

    /**
     * @brief
     * @param id
     */
    void skipIn(const util::Section &id) override;

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

#endif  // SRC_GENIE_FORMAT_FASTQ_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
