/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Provides a reader for FASTA format files with support for indexing and hashing.
 *
 * The `FastaReader` class is designed to facilitate efficient access to genomic sequences stored in
 * FASTA files. It leverages accompanying index (.fai) and hash (e.g., SHA256) files to provide fast lookups
 * and verification of sequence integrity. The class offers functionality to generate these files if they
 * are not available, allowing for seamless integration with downstream bioinformatics tools.
 *
 * This file is part of the GENIE project, a software suite for processing genomic data according to
 * the MPEG-G standard. For more information, see the LICENSE file or visit the project repository:
 * https://github.com/MueFab/genie.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_READER_H_
#define SRC_GENIE_FORMAT_FASTA_READER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <set>
#include <string>
#include "genie/core/meta/external-ref/fasta.h"
#include "genie/core/meta/reference.h"
#include "genie/format/fasta/fai-file.h"
#include "genie/format/fasta/sha256File.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief Handles reading, indexing, and hashing of FASTA files.
 *
 * The `FastaReader` class provides an interface for managing FASTA files, including reading genomic sequences,
 * generating index files (.fai), and creating SHA256 hash files for sequence validation. It can be used as a
 * standalone reader or as part of a larger pipeline for genomic data processing and validation.
 */
class FastaReader {
 private:
    Sha256File hashFile;  //!< @brief Holds the parsed SHA256 checksum file for validating sequences.
    FaiFile fai;  //!< @brief Stores the .fai index file for the FASTA file, enabling fast access to sequence data.
    std::istream* fasta;  //!< @brief Input stream for the main FASTA file.
    std::string path;     //!< @brief Path to the directory containing the FASTA file and related files.

 public:
    /**
     * @brief Constructs a `FastaReader` instance.
     *
     * Initializes the reader with input streams for the FASTA file, its index file (.fai), and its checksum file.
     * The base path is used for resolving any relative paths within the input files.
     *
     * @param fastaFile Input stream for the main FASTA file containing sequence data.
     * @param faiFile Input stream for the .fai index file that provides fast lookups for sequence locations.
     * @param sha256File Input stream for the checksum file containing hash values for each sequence.
     * @param _path The base path for resolving URIs or relative paths in the FASTA index.
     */
    FastaReader(std::istream& fastaFile, std::istream& faiFile, std::istream& sha256File, std::string _path);

    /**
     * @brief Retrieves a map of sequence indices and their corresponding names.
     *
     * This method returns a map where each entry represents a sequence, with its index in the FASTA file as the key
     * and the sequence name as the value. This allows for quick lookups of sequence names based on their position
     * in the index.
     *
     * @return A map where the key is the sequence index and the value is the sequence name.
     */
    [[nodiscard]] std::map<size_t, std::string> getSequences() const;

    /**
     * @brief Gets the length of a specified sequence.
     *
     * Given the name of a sequence, this method returns its total length. It utilizes the FASTA index file to
     * quickly retrieve this information without needing to traverse the entire FASTA file.
     *
     * @param name The name of the sequence for which to retrieve the length.
     * @return The total length of the sequence in bases.
     */
    [[nodiscard]] uint64_t getLength(const std::string& name) const;

    /**
     * @brief Loads a specific section of a sequence.
     *
     * This method extracts a subsequence from the specified sequence in the FASTA file, based on the start
     * and end positions. It uses the .fai index for efficient lookups and retrievals.
     *
     * @param sequence The name of the sequence from which to extract the subsequence.
     * @param start The starting position of the subsequence (0-based).
     * @param end The ending position of the subsequence (0-based, exclusive).
     * @return A string representing the subsequence extracted from the specified range.
     */
    std::string loadSection(const std::string& sequence, uint64_t start, uint64_t end);

    /**
     * @brief Constructs a metadata reference object for the FASTA file.
     *
     * This method constructs and returns a `Reference` object that encapsulates metadata for all the sequences
     * managed by this `FastaReader`. The metadata includes details such as sequence names, lengths, and their
     * respective URIs in the FASTA file.
     *
     * @return A `Reference` object containing metadata for the managed sequences.
     */
    [[nodiscard]] core::meta::Reference getMeta() const;

    /**
     * @brief Generates a .fai index file for a given FASTA file.
     *
     * This static method reads a FASTA file and outputs an index file (.fai) that provides quick access to
     * sequence positions and lengths within the FASTA file.
     *
     * @param fasta Input stream for the main FASTA file to be indexed.
     * @param fai Output stream for the .fai index file to be created.
     */
    static void index(std::istream& fasta, std::ostream& fai);

    /**
     * @brief Computes and stores SHA256 hashes for sequences in a FASTA file.
     *
     * This static method reads a FASTA file and computes SHA256 hashes for each sequence, storing the resulting
     * hashes in a specified output file. This enables verification of sequence integrity.
     *
     * @param fai The index file that maps sequences to their positions in the FASTA file.
     * @param fasta Input stream for the main FASTA file to be hashed.
     * @param hash Output stream for storing the computed SHA256 hashes.
     */
    static void hash(const FaiFile& fai, std::istream& fasta, std::ostream& hash);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_READER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
