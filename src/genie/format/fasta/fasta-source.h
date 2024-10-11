/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `FastaSource` class for generating and managing FASTA files.
 *
 * This file contains the implementation of the `FastaSource` class, which is responsible for
 * creating FASTA files using sequence data retrieved from a reference manager. The class
 * also provides methods to stream sequences to an output file in a thread-safe manner.
 *
 * This file is part of the GENIE project, a software suite for processing genomic data
 * according to the MPEG-G standard. For more details, refer to the LICENSE file or visit
 * the project's repository at: https://github.com/MueFab/genie.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_
#define SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <mutex>
#include <ostream>
#include "genie/core/reference-manager.h"
#include "genie/util/ordered-lock.h"
#include "genie/util/original-source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief Provides methods for creating a FASTA file from sequence data.
 *
 * The `FastaSource` class serves as an interface for generating a FASTA file using data
 * from a `ReferenceManager`. It extends the `OriginalSource` and `Source` interfaces,
 * allowing it to act as a producer of sequence strings that are written to an output stream.
 *
 * The class maintains a map of accumulated sequence lengths to facilitate efficient
 * sequence lookups and writing. Thread-safety is ensured using a `OrderedLock` mechanism.
 */
class FastaSource : public util::OriginalSource, public util::Source<std::string> {
 private:
    std::ostream* outfile;                       //!< @brief Output stream for the FASTA file.
    util::OrderedLock outlock;                   //!< @brief Lock mechanism for synchronized writing.
    core::ReferenceManager* refMgr;              //!< @brief Reference manager to retrieve sequences.
    std::map<std::string, size_t> accu_lengths;  //!< @brief Map to store accumulated lengths for each sequence.
    size_t line_length;                          //!< @brief Line length for formatting sequences in the FASTA file.

 public:
    /**
     * @brief Constructs a `FastaSource` object.
     *
     * Initializes the `FastaSource` with the given output file stream and reference manager.
     * The class uses the reference manager to access and organize sequence data, which is then
     * formatted and written to the specified output stream.
     *
     * @param _outfile Pointer to the output file stream where the FASTA content will be written.
     * @param _refMgr Pointer to the `ReferenceManager` for managing reference sequences.
     */
    FastaSource(std::ostream* _outfile, core::ReferenceManager* _refMgr);

    /**
     * @brief Pumps sequence data from the source into a string buffer.
     *
     * This method is called to retrieve a sequence string from the reference manager, format it
     * according to FASTA specifications, and store it in an output buffer. The method uses a
     * lock to ensure that data is written in a thread-safe manner.
     *
     * @param id The sequence ID being processed. The ID is used to determine the order of writing.
     * @param lock Mutex lock for synchronizing access to shared resources.
     * @return `true` if the pumping was successful, `false` otherwise.
     */
    bool pump(uint64_t& id, std::mutex& lock) override;

    /**
     * @brief Flushes any remaining data to the output stream.
     *
     * This method is called when all sequence data has been processed. It ensures that any
     * remaining data in the buffer is flushed to the output file before the stream is closed.
     *
     * @param id The ID associated with the current flushing operation.
     */
    void flushIn(uint64_t& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
