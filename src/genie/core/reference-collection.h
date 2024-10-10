/**
 * @file reference-collection.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the ReferenceCollection class for managing multiple genomic reference sequences.
 *
 * This file contains the declaration of the `ReferenceCollection` class, which acts as a container for multiple
 * genomic reference sequences. It provides methods to register new references, query sequences by name and range,
 * and retrieve metadata such as coverage and sequence names.
 *
 * @details The `ReferenceCollection` class is designed to manage a collection of references, enabling efficient
 * access to specific sequences and their coverage regions. It is used internally by other GENIE components to
 * handle multiple references and organize them based on their names and positions.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_COLLECTION_H_
#define SRC_GENIE_CORE_REFERENCE_COLLECTION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief Manages a collection of genomic reference sequences.
 *
 * The `ReferenceCollection` class stores and manages multiple reference sequences, providing functionality
 * to query specific sequences by name and range, register new reference sequences, and retrieve metadata
 * about the collection. It uses an internal map to organize references based on their names, enabling
 * efficient access and management.
 */
class ReferenceCollection {
 private:
    std::map<std::string, std::vector<std::unique_ptr<Reference>>>
        refs;  //!< @brief Map of reference names to their corresponding Reference objects.

 public:
    /**
     * @brief Retrieves a specific sequence segment by name and range.
     *
     * This function extracts a sequence segment from the reference identified by the given name,
     * spanning from `_start` to `_end` (inclusive). If the specified range is not valid, an empty
     * string is returned.
     *
     * @param name The name of the reference sequence.
     * @param _start The starting position of the segment (0-based).
     * @param _end The ending position of the segment (inclusive).
     * @return A string containing the sequence segment for the specified range.
     */
    [[nodiscard]] std::string getSequence(const std::string& name, uint64_t _start, uint64_t _end) const;

    /**
     * @brief Retrieves coverage information for a specific reference.
     *
     * This function returns a list of start and end positions representing the regions of the reference
     * sequence that have been covered (i.e., regions that have been loaded or registered). The list
     * contains pairs of positions indicating the start and end of each covered region.
     *
     * @param name The name of the reference sequence.
     * @return A vector of pairs representing the start and end positions of covered regions.
     */
    [[nodiscard]] std::vector<std::pair<size_t, size_t>> getCoverage(const std::string& name) const;

    /**
     * @brief Retrieves a list of all registered reference sequence names.
     *
     * This function returns a vector containing the names of all reference sequences
     * that have been registered in the collection.
     *
     * @return A vector of reference sequence names.
     */
    [[nodiscard]] std::vector<std::string> getSequences() const;

    /**
     * @brief Registers a new reference sequence in the collection.
     *
     * This function registers a new reference sequence by adding the given `Reference` object
     * to the collection. The reference is organized based on its name, and if a reference
     * with the same name already exists, it is appended to the existing collection.
     *
     * @param ref A unique pointer to the new `Reference` object to be registered.
     */
    void registerRef(std::unique_ptr<Reference> ref);

    /**
     * @brief Registers multiple reference sequences in the collection.
     *
     * This function registers multiple reference sequences by accepting a vector of unique pointers
     * to `Reference` objects. The references are added to the collection and organized based on
     * their names. This operation enables batch registration of references.
     *
     * @param ref A vector of unique pointers to `Reference` objects to be registered.
     */
    void registerRef(std::vector<std::unique_ptr<Reference>>&& ref);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_COLLECTION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
