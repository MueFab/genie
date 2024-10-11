/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 *
 * @brief Defines the class for exporting MPEG-G genomic records to FASTQ files.
 *
 * The `Exporter` class is used to convert and output MPEG-G formatted genomic records into FASTQ format.
 * It supports both single-end and paired-end reads, ensuring compatibility with downstream bioinformatics tools
 * that operate on the standard FASTQ format.
 */

#ifndef SRC_GENIE_FORMAT_FASTQ_EXPORTER_H_
#define SRC_GENIE_FORMAT_FASTQ_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include "genie/core/format-exporter.h"
#include "genie/core/record/chunk.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/drain.h"
#include "genie/util/ordered-lock.h"
#include "genie/util/ordered-section.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fastq {

/**
 * @brief Module to export MPEG-G records to FASTQ files.
 *
 * This class converts genomic records stored in the MPEG-G format into the standard FASTQ format.
 * It supports exporting to either a single file for unpaired reads or two separate files for paired-end reads.
 * The `Exporter` ensures that all records are written in the correct order, even when operating in a multithreaded environment.
 */
class Exporter : public core::FormatExporter {
 private:
    std::vector<std::ostream *> file;  //!< @brief Vector of output files (size 1 for single-end, 2 for paired-end).
    util::OrderedLock lock;            //!< @brief Lock to ensure ordered execution in a multithreaded setup.

 public:
    /**
     * @brief Constructor for unpaired FASTQ export.
     *
     * This constructor initializes the `Exporter` for unpaired read data, with a single output file.
     * The input MPEG-G records will be converted and written sequentially to the specified output stream.
     *
     * @param _file_1 Output stream for unpaired reads.
     */
    explicit Exporter(std::ostream &_file_1);

    /**
     * @brief Constructor for paired FASTQ export.
     *
     * This constructor initializes the `Exporter` for paired-end read data, with two separate output files.
     * The input MPEG-G records will be converted and written sequentially to the two specified output streams.
     *
     * @param _file_1 Output stream for the first read in a paired-end read (typically named `R1`).
     * @param _file_2 Output stream for the second read in a paired-end read (typically named `R2`).
     */
    Exporter(std::ostream &_file_1, std::ostream &_file_2);

    /**
     * @brief Skip a specific section during FASTQ export.
     *
     * This method skips the specified section during the conversion process, which is useful when dealing
     * with multithreaded input data that may have been processed out of order. Skipping ensures that the
     * exported data remains aligned with the original order.
     *
     * @param id Section identifier for the data to be skipped.
     */
    void skipIn(const util::Section &id) override;

    /**
     * @brief Process one chunk of MPEG-G records and export them to FASTQ.
     *
     * This method converts a single chunk of MPEG-G formatted genomic records into the FASTQ format and writes them
     * to the appropriate output file(s). For paired-end reads, it writes each read pair to its respective output file.
     * The `flowIn` function is invoked by the framework's multithreaded processing pipeline.
     *
     * @param records Input records in MPEG-G format.
     * @param id Block identifier to ensure ordered output in multithreaded contexts.
     */
    void flowIn(core::record::Chunk &&records, const util::Section &id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fastq

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTQ_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
