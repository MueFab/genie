/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_CHUNK_H_
#define SRC_GENIE_CORE_RECORD_CHUNK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <utility>
#include <vector>
#include "genie/core/record/record.h"
#include "genie/core/reference-manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
class Chunk {
 private:
    std::vector<Record> data;                           //!< @brief
    ReferenceManager::ReferenceExcerpt reference;       //!< @brief
    std::vector<std::pair<size_t, size_t>> refToWrite;  //!< @brief
    size_t refID{};                                     //!< @brief
    stats::PerfStats stats;                             //!< @brief
    bool referenceOnly{false};                          //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    std::vector<Record>& getData();

    /**
     * @brief
     * @return
     */
    ReferenceManager::ReferenceExcerpt& getRef();

    /**
     * @brief
     * @param start
     * @param end
     */
    void addRefToWrite(size_t start, size_t end);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<std::pair<size_t, size_t>>& getRefToWrite() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const ReferenceManager::ReferenceExcerpt& getRef() const;

    /**
     * @brief
     * @param id
     */
    void setRefID(size_t id);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t getRefID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<Record>& getData() const;

    /**
     * @brief
     * @return
     */
    stats::PerfStats& getStats();

    /**
     * @brief
     * @param s
     */
    void setStats(stats::PerfStats&& s);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool isReferenceOnly() const;

    /**
     * @brief
     * @param ref
     */
    void setReferenceOnly(bool ref);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CHUNK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
