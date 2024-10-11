/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines a FASTA format reference handler for extracting sequence data.
 *
 * The `Reference` class provides a bridge between the core reference-handling components of the GENIE library
 * and the FASTA format. It extends the core `Reference` class to support retrieval of genomic subsequences
 * from a FASTA file using the associated `Manager` object.
 *
 * This file is part of the GENIE project, a software suite for processing genomic data according to
 * the MPEG-G standard. For more information, see the LICENSE file or visit the project repository:
 * https://github.com/MueFab/genie.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_REFERENCE_H_
#define SRC_GENIE_FORMAT_FASTA_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/reference.h"
#include "genie/format/fasta/manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief Specialized class for accessing subsequences from a FASTA file.
 *
 * This class extends the core `Reference` class to provide specific implementations for retrieving
 * genomic sequence data from a FASTA file. The FASTA file is managed through a `Manager` object,
 * which handles indexing, loading, and caching of sequences. This class is used internally by the
 * GENIE framework when processing FASTA-based references.
 */
class Reference : public core::Reference {
 private:
    Manager* mgr;  //!< @brief Pointer to the `Manager` handling the underlying FASTA file.

 public:
    /**
     * @brief Constructs a `Reference` object for a specific sequence.
     *
     * This constructor initializes the reference object with the sequence name, its total length, and
     * a pointer to the `Manager` that handles loading and caching of sequence data from the FASTA file.
     *
     * @param name The name of the sequence (e.g., "chr1", "chrX").
     * @param length The total length of the sequence in bases.
     * @param m Pointer to the `Manager` that manages the FASTA file containing this sequence.
     */
    Reference(std::string name, size_t length, Manager* m);

    /**
     * @brief Retrieves a specific subsequence from the reference.
     *
     * This method retrieves a subsequence based on the provided start and end positions (0-based, inclusive).
     * The `Manager` object is used to efficiently access the sequence data using the internal indexing
     * mechanisms of the FASTA file.
     *
     * @param _start The starting position of the subsequence (0-based).
     * @param _end The ending position of the subsequence (0-based, exclusive).
     * @return The requested subsequence as a string.
     */
    std::string getSequence(uint64_t _start, uint64_t _end) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
