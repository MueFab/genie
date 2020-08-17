/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_REFERENCE_COLLECTION_H
#define GENIE_REFERENCE_COLLECTION_H

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ReferenceCollection {
   private:
    std::map<std::string, std::vector<std::unique_ptr<Reference>>> refs;  //!<

   public:
    /**
     *
     * @param name
     * @param _start
     * @param _end
     * @return
     */
    std::string getSequence(const std::string& name, uint64_t _start, uint64_t _end) const;

    /**
     *
     * @param name
     * @return
     */
    std::vector<std::pair<size_t, size_t>> getCoverage(const std::string& name) const;

    /**
     *
     * @return
     */
    std::vector<std::string> getSequences() const;

    /**
     *
     * @param ref
     */
    void registerRef(std::unique_ptr<Reference> ref);

    /**
     *
     * @param ref
     */
    void registerRef(std::vector<std::unique_ptr<Reference>>&& ref);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_REFERENCE_COLLECTION_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------