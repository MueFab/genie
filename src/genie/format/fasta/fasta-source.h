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
#include "genie/util/ordered-lock.h"
#include "genie/util/original-source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 *
 */
class FastaSource : public util::OriginalSource, public util::Source<std::string> {
 private:
    std::ostream* outfile;                       //!<
    util::OrderedLock outlock;                   //!<
    core::ReferenceManager* refMgr;              //!<
    std::map<std::string, size_t> accu_lengths;  //!<
    size_t line_length;                          //!<

 public:
    /**
     *
     * @param _outfile
     * @param _refMgr
     */
    FastaSource(std::ostream* _outfile, core::ReferenceManager* _refMgr);

    /**
     *
     * @param id
     * @param lock
     * @return
     */
    bool pump(uint64_t& id, std::mutex& lock) override;

    /**
     *
     * @param id
     */
    void flushIn(uint64_t& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_FASTA_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
