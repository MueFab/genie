/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
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

namespace genie {
namespace core {

/**
 * @brief
 */
class ReferenceCollection {
 private:
    std::map<std::string, std::vector<std::unique_ptr<Reference>>> refs;  //!< @brief

 public:
    /**
     * @brief
     * @param name
     * @param _start
     * @param _end
     * @return
     */
    std::string getSequence(const std::string& name, uint64_t _start, uint64_t _end) const;

    /**
     * @brief
     * @param name
     * @return
     */
    std::vector<std::pair<size_t, size_t>> getCoverage(const std::string& name) const;

    /**
     * @brief
     * @return
     */
    std::vector<std::string> getSequences() const;

    /**
     * @brief
     * @param ref
     */
    void registerRef(std::unique_ptr<Reference> ref);

    /**
     * @brief
     * @param ref
     */
    void registerRef(std::vector<std::unique_ptr<Reference>>&& ref);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_COLLECTION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
