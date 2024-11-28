/**
 * @file original-source.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the OriginalSource interface for introducing new data into the application.
 *
 * This file contains the declaration of the `OriginalSource` interface, which serves as a base class
 * for components that generate or introduce new data into the application without relying on external
 * input sources. Implementations of this interface are responsible for creating and managing new data
 * entries and signaling the end of data when no more entries are available.
 *
 * @details The `OriginalSource` interface provides a method for pushing new data into the processing
 * pipeline and a mechanism for signaling when the data source has been exhausted. It is intended for
 * use in scenarios where data is generated internally or derived from other operations rather than read
 * from an input file or stream.
 */

#ifndef SRC_GENIE_UTIL_ORIGINAL_SOURCE_H_
#define SRC_GENIE_UTIL_ORIGINAL_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <mutex> //NOLINT

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Interface for introducing new data into the application without external inputs.
 *
 * The `OriginalSource` interface defines a standard for classes that generate or manage
 * new data entries to be processed by the application. It provides a `pump()` method for
 * adding new data entries and a `flushIn()` method for signaling the end of data. Implementing
 * this interface indicates that the class is responsible for being the origin of a new data
 * source within the application.
 */
class OriginalSource {
 public:
    /**
     * @brief Generates or pushes a new data entry into the application.
     *
     * This function is responsible for creating or generating a new data entry and pushing
     * it into the application. It uses the provided `id` to track the position of the data
     * and the given `lock` to ensure thread-safe operations. The function returns a boolean
     * value indicating whether a new data entry was successfully created and pushed.
     *
     * @param id A reference to the identifier for tracking the current data position.
     * @param lock A reference to a mutex used for synchronizing access to shared resources.
     * @return True if a new data entry was generated and pushed successfully, false otherwise.
     */
    virtual bool pump(uint64_t& id, std::mutex& lock) = 0;

    /**
     * @brief Signals that no more data entries will be generated.
     *
     * This function is called when the data source has been exhausted, indicating that no
     * further data entries will be created or pushed into the application. It updates the
     * given position counter to reflect the final position of the data source.
     *
     * @param pos A reference to the position counter to be updated.
     */
    virtual void flushIn(uint64_t& pos) = 0;

    /**
     * @brief Virtual destructor for safe polymorphic use.
     *
     * Ensures that derived classes are properly destructed when handled through a pointer
     * to the `OriginalSource` interface.
     */
    virtual ~OriginalSource() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ORIGINAL_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
