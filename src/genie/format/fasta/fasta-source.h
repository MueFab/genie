/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_
#define SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include "genie/core/reference-manager.h"
#include "genie/util/ordered_lock.h"
#include "genie/util/original_source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 * @brief
 */
class FastaSource : public util::OriginalSource, public util::Source<std::string> {
 private:
    std::ostream* outfile;                       //!< @brief
    util::OrderedLock outlock;                   //!< @brief
    core::ReferenceManager* refMgr;              //!< @brief
    std::map<std::string, size_t> accu_lengths;  //!< @brief
    size_t line_length;                          //!< @brief

 public:
    /**
     * @brief
     * @param _outfile
     * @param _refMgr
     */
    FastaSource(std::ostream* _outfile, core::ReferenceManager* _refMgr);

    /**
     * @brief
     * @param id
     * @param lock
     * @return
     */
    bool Pump(uint64_t& id, std::mutex& lock) override;

    /**
     * @brief
     * @param id
     */
    void FlushIn(uint64_t& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
