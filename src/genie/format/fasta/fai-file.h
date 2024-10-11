/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `FaiFile` class for handling FASTA index files (.fai).
 *
 * The `FaiFile` class is used to parse and interact with FASTA index files, typically
 * associated with genomic data stored in FASTA format. It provides methods to efficiently
 * access sequence information such as offsets, lengths, and positions, which can be used
 * for random access within large FASTA files.
 *
 * This file is part of GENIE, a software suite for processing genomic data according to the
 * MPEG-G standard. See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_FAI_FILE_H_
#define SRC_GENIE_FORMAT_FASTA_FAI_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <istream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief Represents a FASTA index file (.fai) for random access to sequences.
 *
 * The `FaiFile` class is used to parse and represent FASTA index files, allowing quick
 * lookup of sequence information such as names, lengths, offsets, and line formatting
 * details. Each sequence in the index is stored as a `FaiSequence` object, which contains
 * metadata about the sequence's position and formatting in the associated FASTA file.
 */
class FaiFile {
 public:
    /**
     * @brief Represents a single sequence entry in the FASTA index file.
     *
     * Each `FaiSequence` object corresponds to a specific sequence in a FASTA file, containing
     * information such as its name, length, file offset, and formatting parameters for efficient
     * extraction and manipulation.
     */
    struct FaiSequence {
        std::string name;  //!< @brief The name of the sequence.
        uint64_t length;   //!< @brief Length of the sequence in bases.
        uint64_t offset;   //!< @brief File offset where the sequence starts.
        size_t linebases;  //!< @brief Number of bases per line in the sequence.
        size_t linewidth;  //!< @brief Total line width including newline characters.

        /**
         * @brief Constructs a `FaiSequence` from an input stream.
         *
         * This constructor reads a line from the given stream and populates the `FaiSequence`
         * attributes based on the FASTA index file format.
         *
         * @param stream Input stream to read from (e.g., a .fai file).
         */
        explicit FaiSequence(std::istream& stream);

        /**
         * @brief Default constructor for `FaiSequence`.
         *
         * Initializes an empty `FaiSequence` object with default values.
         */
        FaiSequence();
    };

    /**
     * @brief Adds a new sequence entry to the `FaiFile`.
     *
     * Inserts a `FaiSequence` object into the index map for lookup and access.
     *
     * @param seq The `FaiSequence` object to add.
     */
    void addSequence(const FaiSequence& seq);

    /**
     * @brief Constructs a `FaiFile` from an input stream.
     *
     * Parses the entire input stream and populates the `FaiFile` object with sequence entries.
     *
     * @param stream Input stream containing a FASTA index (.fai) file.
     */
    explicit FaiFile(std::istream& stream);

    /**
     * @brief Default constructor for `FaiFile`.
     *
     * Initializes an empty `FaiFile` object.
     */
    FaiFile() = default;

    /**
     * @brief Retrieves the file position of a specific base in a sequence.
     *
     * Converts the base position within a sequence to the corresponding byte offset
     * in the FASTA file for efficient random access.
     *
     * @param sequence The name of the sequence to look up.
     * @param position The position within the sequence (0-based).
     * @return The byte offset in the FASTA file corresponding to the given base position.
     */
    [[nodiscard]] uint64_t getFilePosition(const std::string& sequence, uint64_t position) const;

    /**
     * @brief Retrieves a map of sequence indices.
     *
     * Provides a map of indices to sequence names for quick lookup and access.
     *
     * @return A map where keys are sequence indices and values are sequence names.
     */
    [[nodiscard]] std::map<size_t, std::string> getSequences() const;

    /**
     * @brief Checks if the specified sequence exists in the index.
     *
     * Determines if a given sequence name is present in the current index.
     *
     * @param seq The name of the sequence to check.
     * @return `true` if the sequence exists, `false` otherwise.
     */
    [[nodiscard]] bool containsSequence(const std::string& seq) const;

    /**
     * @brief Retrieves the length of a specified sequence.
     *
     * Looks up and returns the length of the given sequence from the index.
     *
     * @param seq The name of the sequence to look up.
     * @return Length of the sequence in bases.
     */
    [[nodiscard]] uint64_t getLength(const std::string& seq) const;

 private:
    std::map<std::string, FaiSequence> seqs;  //!< @brief Map of sequence names to `FaiSequence` objects.
    std::map<size_t, std::string> indices;    //!< @brief Map of indices to sequence names.

    /**
     * @brief Overloads the stream insertion operator for `FaiFile`.
     *
     * Outputs the contents of the `FaiFile` object to a stream in a readable format.
     *
     * @param stream Output stream to write to.
     * @param file The `FaiFile` object to print.
     * @return Reference to the modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& stream, const FaiFile& file);
};

/**
 * @brief Overloads the stream insertion operator for `FaiSequence`.
 *
 * Outputs the details of a `FaiSequence` object to the given stream.
 *
 * @param stream Output stream to write to.
 * @param file The `FaiSequence` object to print.
 * @return Reference to the modified output stream.
 */
std::ostream& operator<<(std::ostream& stream, const FaiFile::FaiSequence& file);

/**
 * @brief Overloads the stream insertion operator for `FaiFile`.
 *
 * Outputs the details of a `FaiFile` object to the given stream.
 *
 * @param stream Output stream to write to.
 * @param file The `FaiFile` object to print.
 * @return Reference to the modified output stream.
 */
std::ostream& operator<<(std::ostream& stream, const FaiFile& file);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_FAI_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
