/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MANAGER_H
#define GENIE_MANAGER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/reference.h>
#include <reference-source.h>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include "reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 *
 */
class Manager : public core::ReferenceSource {
    FastaReader reader;
    std::mutex readerMutex;

    /**
     *
     * @param fasta
     * @param fai
     */
    Manager(std::istream& fasta, std::istream& fai);

   public:

    /**
     *
     * @return
     */
    std::set<std::string> getSequences() const;

    /**
     *
     * @param seq
     * @return
     */
    uint64_t getLength(const std::string& seq) const;

    /**
     *
     * @param sequence
     * @param start
     * @param end
     * @return
     */
    std::string getRef(const std::string& sequence, uint64_t start, uint64_t end);

    /**
     *
     * @return
     */
    std::vector<std::unique_ptr<core::Reference>> generateRefHandles() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MANAGER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------