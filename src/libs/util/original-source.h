/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ORIGINAL_SOURCE_H
#define GENIE_ORIGINAL_SOURCE_H

/**
 * @brief Implementing this interface signals the ability to introduce new data into the application without any input
 */
class OriginalSource {
   public:
    /**
     * @brief Spawn some data
     * @param current block id
     * @return True if more data is available, false otherwise
     */
    virtual bool pump(size_t id) = 0;

    /**
     * @brief Signal end of data.
     */
    virtual void dryIn() = 0;

    /**
     * @brief For inheritance
     */
    virtual ~OriginalSource() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ORIGINAL_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------