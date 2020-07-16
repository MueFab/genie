/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef UTIL_FASTA_FILE_READER_H_
#define UTIL_FASTA_FILE_READER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/runtime-exception.h>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "fai-file.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 *
 */
class FastaReader {
   private:
    FaiFile fai;          //!<
    std::istream* fasta;  //!<

   public:
    /**
     *
     * @param fastaFile
     * @param faiFile
     */
    FastaReader(std::istream& fastaFile, std::istream& faiFile);

    /**
     *
     * @return
     */
    std::set<std::string> getSequences() const;

    /**
     *
     * @param name
     * @return
     */
    uint64_t getLength(const std::string& name) const;

    /**
     *
     * @param sequence
     * @param start
     * @param end
     * @return
     */
    std::string loadSection(const std::string& sequence, uint64_t start, uint64_t end);

    /**
     *
     * @param fasta
     * @param fai
     */
    static void index(std::istream& fasta, std::ostream& fai);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // UTIL_FASTA_FILE_READER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------