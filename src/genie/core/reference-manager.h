/**
 * @file reference-manager.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the ReferenceManager class for managing genomic reference sequences.
 *
 * This file contains the declaration of the `ReferenceManager` class, which handles loading,
 * caching, and accessing genomic reference sequences. It provides functionality for managing
 * large reference datasets, maintaining efficient memory usage through caching, and retrieving
 * specific portions of the reference sequences as needed.
 *
 * @details The `ReferenceManager` class is designed to facilitate the efficient management
 * of genomic reference sequences in the GENIE framework. It supports the loading and caching
 * of reference chunks, mapping references to unique identifiers, and providing fine-grained
 * access to portions of the reference sequences through the `ReferenceExcerpt` structure.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_MANAGER_H_
#define SRC_GENIE_CORE_REFERENCE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/meta/reference.h"
#include "genie/core/reference-collection.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief Manages loading, caching, and access to genomic reference sequences.
 *
 * The `ReferenceManager` class is responsible for managing genomic reference sequences
 * by loading them into memory, caching portions of the references, and providing efficient
 * access to the sequences. It uses a caching mechanism to optimize memory usage when dealing
 * with large references and supports both ID-based and name-based reference lookups.
 */
class ReferenceManager {
 private:
    ReferenceCollection mgr;  //!< @brief Collection of all managed reference sequences.

    /**
     * @brief Structure representing a single cached chunk of a reference.
     */
    struct CacheLine {
        std::shared_ptr<const std::string> chunk;  //!< @brief The actual reference chunk data.
        std::weak_ptr<const std::string> memory;   //!< @brief Weak reference to the chunk for memory management.
        std::mutex loadMutex;                      //!< @brief Mutex for synchronizing access to the chunk.
    };

    std::map<std::string, std::vector<std::unique_ptr<CacheLine>>>
        data;                                              //!< @brief Map from reference names to cached data.
    std::map<size_t, std::string> indices;                 //!< @brief Map from reference IDs to reference names.
    std::deque<std::pair<std::string, size_t>> cacheInfo;  //!< @brief LRU cache information for managing chunks.
    std::mutex cacheInfoLock;                              //!< @brief Mutex protecting the cache information.
    uint64_t cacheSize;                                    //!< @brief Maximum cache size in bytes.
    static const uint64_t CHUNK_SIZE;                      //!< @brief Size of each reference chunk in bytes.

    /**
     * @brief Marks a reference chunk as recently used in the cache.
     *
     * This function updates the cache information to indicate that the specified
     * chunk of a reference has been accessed, ensuring that it is not removed
     * from the cache prematurely.
     *
     * @param name The name of the reference sequence.
     * @param num The chunk index within the reference sequence.
     */
    void touch(const std::string& name, size_t num);

 public:
    /**
     * @brief Constructs a `ReferenceManager` with a specified cache size.
     *
     * @param csize The maximum cache size in bytes.
     */
    explicit ReferenceManager(size_t csize);

    /**
     * @brief Verifies if a given reference ID is valid.
     *
     * This function checks if the specified reference ID is known to the `ReferenceManager`.
     *
     * @param id The reference ID to validate.
     */
    void validateRefID(size_t id);

    /**
     * @brief Maps a reference name to its unique identifier.
     *
     * This function maps the given reference name to its corresponding identifier.
     * If the reference is not already known, a new identifier is generated.
     *
     * @param ref The name of the reference sequence.
     * @return The unique identifier associated with the reference.
     */
    size_t ref2ID(const std::string& ref);

    /**
     * @brief Maps a reference identifier to its name.
     *
     * This function returns the name of the reference sequence associated with
     * the given identifier.
     *
     * @param id The reference identifier.
     * @return The name of the reference sequence.
     */
    std::string ID2Ref(size_t id);

    /**
     * @brief Checks if a reference is known.
     *
     * This function checks if the reference with the given identifier is
     * known to the `ReferenceManager`.
     *
     * @param id The reference identifier.
     * @return True if the reference is known, false otherwise.
     */
    bool refKnown(size_t id);

    /**
     * @brief Retrieves the size of each reference chunk.
     *
     * This function returns the fixed size of the reference chunks managed
     * by the `ReferenceManager`.
     *
     * @return The size of each chunk in bytes.
     */
    static size_t getChunkSize();

    /**
     * @brief Adds a new reference to the manager.
     *
     * This function adds a new reference sequence to the manager and associates
     * it with a specific index.
     *
     * @param index The index for the reference.
     * @param ref The reference sequence to add.
     */
    void addRef(size_t index, std::unique_ptr<Reference> ref);

    /**
     * @brief Loads a reference chunk at a specific position.
     *
     * @param name The name of the reference sequence.
     * @param pos The position within the reference.
     * @return A shared pointer to the loaded reference chunk.
     */
    std::shared_ptr<const std::string> loadAt(const std::string& name, size_t pos);

    /**
     * @brief Retrieves the coverage of a reference sequence.
     *
     * This function returns a list of ranges that indicate the covered regions
     * of a reference sequence.
     *
     * @param name The name of the reference sequence.
     * @return A vector of start and end positions representing covered regions.
     */
    [[nodiscard]] std::vector<std::pair<size_t, size_t>> getCoverage(const std::string& name) const;

    /**
     * @brief Retrieves the list of reference sequence names.
     *
     * @return A vector of reference sequence names.
     */
    [[nodiscard]] std::vector<std::string> getSequences() const;

    /**
     * @brief Retrieves the length of a reference sequence.
     *
     * This function returns the length of the reference sequence with the given name.
     *
     * @param name The name of the reference sequence.
     * @return The length of the reference sequence.
     */
    [[nodiscard]] size_t getLength(const std::string& name) const;

    /**
     * @brief Stores a portion of a reference sequence.
     *
     * The `ReferenceExcerpt` structure is used to represent a contiguous portion
     * of a reference sequence, potentially spanning multiple chunks.
     */
    struct ReferenceExcerpt {
     private:
        std::string ref_name;  //!< @brief Name of the reference sequence.
        size_t global_start;   //!< @brief Start position of the excerpt in the reference.
        size_t global_end;     //!< @brief End position of the excerpt in the reference.
        std::vector<std::shared_ptr<const std::string>> data;  //!< @brief Cached data chunks for the excerpt.

        static const std::shared_ptr<const std::string> UNDEF_PAGE;  //!< @brief Placeholder for unmapped chunks.

     public:
        /**
         * @brief Constructs an empty `ReferenceExcerpt`.
         */
        ReferenceExcerpt();

        /**
         * @brief Constructs a `ReferenceExcerpt` with the specified reference name and range.
         *
         * @param name The name of the reference sequence.
         * @param start The start position of the excerpt.
         * @param end The end position of the excerpt.
         */
        ReferenceExcerpt(std::string name, size_t start, size_t end);

        /**
         * @brief Checks if the `ReferenceExcerpt` is empty.
         *
         * This function checks if the `ReferenceExcerpt` contains any data.
         *
         * @return True if the excerpt is empty, false otherwise.
         */
        [[nodiscard]] bool isEmpty() const;

        /**
         * @brief Merges another excerpt into this one.
         *
         * This function combines the data of another `ReferenceExcerpt` into this one,
         * expanding the range if necessary.
         *
         * @param e The `ReferenceExcerpt` to merge into this one.
         */
        void merge(ReferenceExcerpt& e);

        /**
         * @brief Retrieves the global start position of the excerpt.
         *
         * @return The global start position.
         */
        [[nodiscard]] size_t getGlobalStart() const;

        /**
         * @brief Retrieves the global end position of the excerpt.
         *
         * @return The global end position.
         */
        [[nodiscard]] size_t getGlobalEnd() const;

        /**
         * @brief Retrieves the name of the reference sequence.
         *
         * @return The name of the reference sequence.
         */
        [[nodiscard]] const std::string& getRefName() const;

        /**
         * @brief Retrieves a specific chunk at the given position.
         *
         * @param pos The position within the reference sequence.
         * @return A shared pointer to the reference chunk.
         */
        [[nodiscard]] std::shared_ptr<const std::string> getChunkAt(size_t pos) const;

        /**
         * @brief Maps a chunk at a specific position.
         *
         * This function associates a given reference chunk with a specific position
         * within the excerpt.
         *
         * @param pos The position to map the chunk to.
         * @param dat The shared pointer to the chunk data.
         */
        void mapChunkAt(size_t pos, std::shared_ptr<const std::string> dat);

        /**
         * @brief Extends the excerpt to a new end position.
         *
         * @param newEnd The new end position for the excerpt.
         */
        void extend(size_t newEnd);

        /**
         * @brief Checks if a given page is mapped.
         *
         * This function returns true if the specified page is mapped in the excerpt.
         *
         * @param page The shared pointer to the page.
         * @return True if the page is mapped, false otherwise.
         */
        static bool isMapped(const std::shared_ptr<const std::string>& page);

        /**
         * @brief Checks if a position is mapped in the excerpt.
         *
         * @param pos The position to check.
         * @return True if the position is mapped, false otherwise.
         */
        [[nodiscard]] bool isMapped(size_t pos) const;

        /**
         * @brief Retrieves the placeholder page for unmapped chunks.
         *
         * This function returns a shared pointer to a constant string that is used
         * as a placeholder for chunks that are not yet mapped in the `ReferenceExcerpt`.
         *
         * @return A shared pointer to the placeholder unmapped page.
         */
        static const std::shared_ptr<const std::string>& undef_page();

        /**
         * @brief Retrieves a string representation of a specific range.
         *
         * This function extracts a string representing a portion of the reference sequence.
         *
         * @param start The start position.
         * @param end The end position.
         * @return The string representation of the specified range.
         */
        [[nodiscard]] std::string getString(size_t start, size_t end) const;

        /**
         * @brief Iterator class for stepping through reference chunks.
         */
        struct Stepper {
         private:
            std::vector<std::shared_ptr<const std::string>>::const_iterator
                startVecIt;                                                         //!< @brief Start of the chunk data.
            std::vector<std::shared_ptr<const std::string>>::const_iterator vecIt;  //!< @brief Current chunk iterator.
            std::vector<std::shared_ptr<const std::string>>::const_iterator
                endVecIt;           //!< @brief End of the chunk data.
            size_t stringPos;       //!< @brief Position within the current string.
            const char* curString;  //!< @brief Pointer to the current string data.

         public:
            /**
             * @brief Constructs a `Stepper` for a given excerpt.
             *
             * @param e The `ReferenceExcerpt` to step through.
             */
            explicit Stepper(const ReferenceExcerpt& e);

            /**
             * @brief Advances the stepper by a specified offset.
             *
             * @param off The offset to advance.
             */
            void inc(size_t off = 1);

            /**
             * @brief Checks if the stepper has reached the end.
             *
             * @return True if at the end, false otherwise.
             */
            [[nodiscard]] bool end() const;

            /**
             * @brief Retrieves the current character.
             *
             * @return The character at the current position.
             */
            [[nodiscard]] char get() const;
        };

        /**
         * @brief Retrieves a stepper for iterating over the excerpt.
         *
         * @return A `Stepper` object for iterating over the excerpt.
         */
        [[nodiscard]] Stepper getStepper() const;
    };

    /**
     * @brief Loads a portion of a reference sequence.
     *
     * This function loads a specified range of a reference sequence into a `ReferenceExcerpt`.
     *
     * @param name The name of the reference sequence.
     * @param start The start position.
     * @param end The end position.
     * @return A `ReferenceExcerpt` containing the loaded range.
     */
    ReferenceExcerpt load(const std::string& name, size_t start, size_t end);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
