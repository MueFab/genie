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
#include <map>
#include <memory>
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
    /**
     *
     */
    struct FastaChunk {
        std::shared_ptr<const std::string> chunk;  //!<
    };
    FastaReader reader;                                       //!<
    std::map<std::string, std::vector<FastaChunk>> data;      //!<
    std::vector<std::pair<std::string, uint64_t>> cacheList;  //!<
    static const uint64_t CHUNK_SIZE = 1 * 1024 * 1024;       //!<
    static const uint64_t CACHE_SIZE = 16;                    //!<

    /**
     *
     * @param fasta
     * @param fai
     */
    Manager(std::istream& fasta, std::istream& fai);

    /**
     *
     * @param seq
     * @param id
     * @return
     */
    std::pair<std::string, uint64_t> updateCacheList(const std::string& seq, size_t id);

    /**
     *
     * @param sequence
     * @param chunk
     * @return
     */
    std::shared_ptr<const std::string> getChunk(const std::string& sequence, size_t chunk);

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