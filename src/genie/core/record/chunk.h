/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_CHUNK_H_
#define SRC_GENIE_CORE_RECORD_CHUNK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <utility>
#include <vector>
#include "genie/core/record/record.h"
#include "genie/core/reference-manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

/**
 *
 */
class Chunk {
 private:
    std::vector<Record> data;                           //!<
    ReferenceManager::ReferenceExcerpt reference;       //!<
    std::vector<std::pair<size_t, size_t>> refToWrite;  //!<
    size_t refID;                                       //!<
    stats::PerfStats stats;                             //!<
    bool referenceOnly{false};                          //!<

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
    ReferenceManager::ReferenceExcerpt& getRef();

    /**
     *
     * @param start
     * @param end
     */
    void addRefToWrite(size_t start, size_t end);

    /**
     *
     * @return
     */
    const std::vector<std::pair<size_t, size_t>>& getRefToWrite() const;

    /**
     *
     * @return
     */
    const ReferenceManager::ReferenceExcerpt& getRef() const;

    /**
     *
     * @param id
     */
    void setRefID(size_t id);

    /**
     *
     * @return
     */
    size_t getRefID() const;

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

    /**
     *
     * @return
     */
    bool isReferenceOnly() const;

    /**
     *
     * @param ref
     */
    void setReferenceOnly(bool ref);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CHUNK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
