/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_READER_H_
#define SRC_GENIE_FORMAT_FASTA_READER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <set>
#include <string>
#include "genie/format/fasta/fai-file.h"

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

#endif  // SRC_GENIE_FORMAT_FASTA_READER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
