/**
 * @file exporter.h
 * @brief Header file for the MGREC format exporter module in GENIE.
 *
 * This file defines the `Exporter` class used for writing records into the MGREC format.
 * The exporter handles record formatting, manages the writing process, and maintains
 * thread safety through an ordered lock mechanism.
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGREC_EXPORTER_H_
#define SRC_GENIE_FORMAT_MGREC_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/format-exporter.h"
#include "genie/core/record/chunk.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/bit-writer.h"
#include "genie/util/drain.h"
#include "genie/util/ordered-lock.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgrec {

/**
 * @class Exporter
 * @brief MGREC format exporter for writing records to a file.
 *
 * The `Exporter` class is designed to handle the export of records into the MGREC format,
 * utilizing a bit-wise writer to serialize and output data. It ensures thread safety
 * through an ordered lock mechanism, allowing parallel processing and synchronized
 * record writing.
 */
class Exporter : public core::FormatExporter {
    util::BitWriter writer;  //!< Bit writer for serializing records
    util::OrderedLock lock;  //!< Ordered lock for thread-safe writing

 public:
    /**
     * @brief Construct an MGREC exporter.
     * @param _file_1 Output stream for the MGREC file.
     */
    explicit Exporter(std::ostream& _file_1);

    /**
     * @brief Process and write a chunk of records to the MGREC file.
     * @param t Chunk of records to be written.
     * @param id Section identifier for synchronization.
     */
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;

    /**
     * @brief Skip processing of a given section.
     * @param id Section identifier to be skipped.
     */
    void skipIn(const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgrec

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGREC_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
