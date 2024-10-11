/**
 * @file
 * @brief Defines the `Exporter` class for exporting sequences in FASTA format.
 *
 * The `Exporter` class is responsible for converting internal genomic sequence representations
 * into the standard FASTA format and writing them to an output stream. This class is part of the
 * GENIE project and interacts closely with the `ReferenceManager` to access and format reference
 * sequences. The `Exporter` class supports multi-threaded exporting for enhanced performance.
 *
 * @copyright
 * This file is part of GENIE. See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_EXPORTER_H_
#define SRC_GENIE_FORMAT_FASTA_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/format-exporter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief Exports genomic data to the FASTA format.
 *
 * The `Exporter` class takes genomic sequences from internal representations and writes them
 * in FASTA format to the specified output stream. It uses the `ReferenceManager` to retrieve
 * the necessary reference information and supports parallel processing to speed up the
 * export operation when working with large datasets.
 */
class Exporter : public core::FormatExporter {
 private:
    core::ReferenceManager* refMgr;  //!< @brief Pointer to the `ReferenceManager` for sequence data access.
    std::ostream* outfile;           //!< @brief Output stream where the FASTA file will be written.
    size_t num_threads;              //!< @brief Number of threads to use for parallel export operations.

 public:
    /**
     * @brief Constructs an `Exporter` object for FASTA format.
     *
     * Initializes the `Exporter` with the given reference manager, output stream, and thread count.
     *
     * @param _refMgr Pointer to the `ReferenceManager` to retrieve reference sequences.
     * @param out Pointer to the output stream where the FASTA data will be written.
     * @param _num_threads Number of threads to use for parallel processing.
     */
    Exporter(core::ReferenceManager* _refMgr, std::ostream* out, size_t _num_threads);

    /**
     * @brief Flushes the internal buffers to the output stream.
     *
     * This method ensures that all pending data is written to the output stream, maintaining
     * consistency in the exported FASTA file.
     *
     * @param id The current identifier of the dataset being processed.
     */
    void flushIn(uint64_t& id) override;

    /**
     * @brief Processes a chunk of genomic records and exports them to the FASTA file.
     *
     * This method converts the given chunk of genomic records into the FASTA format and writes
     * it to the output stream. It uses the `Section` parameter to manage specific regions of
     * the dataset.
     *
     * @param c The chunk of genomic records to be processed and exported.
     * @param s The section descriptor indicating the range and properties of the chunk.
     */
    void flowIn(core::record::Chunk&& c, const util::Section& s) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
