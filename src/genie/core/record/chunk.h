/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CHUNK_H
#define GENIE_CHUNK_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/reference-manager.h>
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
    ReferenceManager::ReferenceExcerpt reference;
    std::vector<std::pair<size_t, size_t>> refToWrite;
    size_t refID;
    stats::PerfStats stats;    //!<
   public:
    /**
     *
     * @return
     */
    std::vector<Record>& getData();

    ReferenceManager::ReferenceExcerpt& getRef(){
        return reference;
    }

    void addRefToWrite(size_t start, size_t end) {
        refToWrite.emplace_back(start, end);
    }

    const std::vector<std::pair<size_t, size_t>>& getRefToWrite() const {
        return refToWrite;
    }

    const ReferenceManager::ReferenceExcerpt& getRef() const{
        return reference;
    }

    void setRefID(size_t id) {
        refID = id;
    }

    size_t getRefID() const {
        return refID;
    }

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