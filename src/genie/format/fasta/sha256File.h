/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the SHA-256 file handler for FASTA references.
 *
 * The `Sha256File` class is used to handle and generate SHA-256 checksums for genomic sequences in FASTA files.
 * It provides methods for reading, writing, and calculating checksums, as well as utilities for converting
 * hexadecimal strings to byte sequences. This functionality is essential for verifying the integrity and
 * authenticity of reference data in genomic processing workflows.
 *
 * This file is part of the GENIE project, a software suite for processing genomic data according to
 * the MPEG-G standard. For more information, see the LICENSE file or visit the project repository:
 * https://github.com/MueFab/genie.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_SHA256FILE_H_
#define SRC_GENIE_FORMAT_FASTA_SHA256FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "picosha2/picosha2.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief Class for handling SHA-256 checksums associated with FASTA sequences.
 *
 * This class is responsible for reading, writing, and computing SHA-256 checksums of genomic sequences
 * in FASTA files. It is used to verify the integrity of reference sequences and to ensure that the
 * genomic data has not been altered or corrupted during processing.
 */
class Sha256File {
 private:
    std::vector<std::pair<std::string, std::string>>
        data;  //!< @brief Vector to store sequence names and their checksums.

    /**
     * @brief Reads a SHA-256 checksum file into memory.
     *
     * This static helper function reads the content of a SHA-256 checksum file, parsing each line
     * to extract the sequence name and its corresponding SHA-256 checksum value.
     *
     * @param file Input stream from which to read the SHA-256 file.
     * @return A vector of pairs, where each pair contains a sequence name and its SHA-256 checksum.
     */
    static std::vector<std::pair<std::string, std::string>> read(std::istream& file);

 public:
    /**
     * @brief Constructs a `Sha256File` object by reading from the given stream.
     *
     * This constructor initializes the `Sha256File` object by reading an existing SHA-256 checksum
     * file from the provided input stream and storing the values in the `data` member variable.
     *
     * @param stream Input stream from which to read the SHA-256 checksum values.
     */
    explicit Sha256File(std::istream& stream) { data = read(stream); }

    /**
     * @brief Retrieves the internal data containing sequence names and their SHA-256 checksums.
     *
     * @return A constant reference to the internal vector containing pairs of sequence names and checksums.
     */
    [[nodiscard]] const std::vector<std::pair<std::string, std::string>>& getData() const { return data; }

    /**
     * @brief Writes the SHA-256 checksum values to the specified output stream.
     *
     * This function writes the given SHA-256 checksum data to the specified output stream in the format:
     * `<checksum>  <sequence_name>`.
     *
     * @param file Output stream to which the checksum data should be written.
     * @param val Vector of pairs containing sequence names and their corresponding SHA-256 checksums.
     */
    static void write(std::ostream& file, const std::vector<std::pair<std::string, std::string>>& val);

    /**
     * @brief Converts a hexadecimal string into a byte sequence.
     *
     * This utility function takes a hexadecimal string (e.g., "a3f5") and converts it into its
     * equivalent byte representation (e.g., "\xA3\xF5"). This is useful when handling raw SHA-256
     * checksums.
     *
     * @param hex The input hexadecimal string.
     * @return A string containing the equivalent byte sequence.
     */
    static std::string hex2bytes(const std::string& hex);

    /**
     * @brief Computes the SHA-256 checksum for a specific section of a file.
     *
     * This function calculates the SHA-256 checksum of a specific segment of the file, defined
     * by a starting position and a length. It is useful for generating or verifying checksums
     * for individual sequences within a larger FASTA file.
     *
     * @param file Input stream representing the file to hash.
     * @param posStart The starting position within the file (in bytes).
     * @param length The length of the segment to hash (in bytes).
     * @return A string representing the SHA-256 checksum in hexadecimal format.
     */
    static std::string hash(std::istream& file, size_t posStart, size_t length);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_SHA256FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
