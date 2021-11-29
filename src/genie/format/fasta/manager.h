/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_MANAGER_H_
#define SRC_GENIE_FORMAT_FASTA_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <set>
#include <string>
#include <vector>
#include "genie/core/reference-source.h"
#include "genie/format/fasta/reader.h"

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

 private:
    std::vector<std::unique_ptr<core::Reference>> generateRefHandles();

 public:
    /**
     *
     * @param fasta
     * @param fai
     * @param mgr
     */
    Manager(std::istream& fasta, std::istream& fai, core::ReferenceManager* mgr);

    /**
     *
     * @return
     */
    std::map<size_t, std::string> getSequences() const;

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
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
