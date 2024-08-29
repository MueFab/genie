/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_MANAGER_H_
#define SRC_GENIE_FORMAT_FASTA_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "genie/core/reference-source.h"
#include "genie/format/fasta/reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief
 */
class Manager : public core::ReferenceSource {
    FastaReader reader;
    std::mutex readerMutex;

 private:
    std::vector<std::unique_ptr<core::Reference>> generateRefHandles();

 public:
    /**
     * @brief
     * @param fasta
     * @param fai
     * @param sha
     * @param mgr
     * @param path
     */
    Manager(std::istream& fasta, std::istream& fai, std::istream& sha, core::ReferenceManager* mgr, std::string path);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::map<size_t, std::string> getSequences() const;

    /**
     * @brief
     * @param seq
     * @return
     */
    [[nodiscard]] uint64_t getLength(const std::string& seq) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] genie::core::meta::Reference getMeta() const override;

    /**
     * @brief
     * @param sequence
     * @param start
     * @param end
     * @return
     */
    std::string getRef(const std::string& sequence, uint64_t start, uint64_t end);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
