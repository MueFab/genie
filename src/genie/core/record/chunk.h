/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CHUNK_H
#define GENIE_CHUNK_H

// ---------------------------------------------------------------------------------------------------------------------

#include "record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

/**
 *
 */
class Chunk {
   private:
    std::vector<Record> data;  //!<
    stats::PerfStats stats;    //!<
   public:
    /**
     *
     * @return
     */
    std::vector<Record>& getData();

    /**
     *
     * @return
     */
    const std::vector<Record>& getData() const;

    /**
     *
     * @return
     */
    stats::PerfStats& getStats();

    /**
     *
     * @param s
     */
    void setStats(stats::PerfStats&& s);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CHUNK_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------