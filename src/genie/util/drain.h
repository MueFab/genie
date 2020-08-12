/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DRAIN_H
#define GENIE_DRAIN_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief A section of genomic record read from the input data sources.
 */
struct Section {
    size_t start;     //!< @brief ID of first record
    size_t length;    //!< @brief Number of records
    bool strongSkip;  //!< @brief If hte pipeline branches and branches are merged again, anly skipIn() with strongSkip
                      //!< == true will pass through the merging point.
};

/**
 * @brief By implementing this interface, a genie module signals the ability to process data of type TYPE.
 * @tparam TYPE Input data type which is accepted.
 */
template <typename TYPE>
class Drain {
   public:
    /**
     * @brief Accept one chunk of data.
     * @param t Input data chunk
     * @param id Current block identifier (for multithreading)
     */
    virtual void flowIn(TYPE&& t, const Section& id) = 0;

    /**
     * @brief Signals that no further data (and calls to flowIn) will be available.
     */
    virtual void flushIn(size_t& pos) = 0;

    /**
     * @brief For inheritance.
     */
    virtual ~Drain() = default;

    /**
     * @brief Signals that a chunk of data won't be delivered using flowIn()
     * @param id Block position of skipped data.
     */
    virtual void skipIn(const Section& id) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DRAIN_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
