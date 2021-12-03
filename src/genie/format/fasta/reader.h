/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_READER_H_
#define SRC_GENIE_FORMAT_FASTA_READER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <set>
#include <string>
#include "genie/core/meta/external-ref/fasta.h"
#include "genie/core/meta/reference.h"
#include "genie/format/fasta/fai-file.h"
#include "genie/format/fasta/sha256File.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 *
 */
class FastaReader {
 private:
    Sha256File hashFile;
    FaiFile fai;          //!<
    std::istream* fasta;  //!<
    std::string path;

 public:
    /**
     *
     * @param fastaFile
     * @param faiFile
     */
    FastaReader(std::istream& fastaFile, std::istream& faiFile, std::istream& sha256File, std::string _path);

    /**
     *
     * @return
     */
    std::map<size_t, std::string> getSequences() const;

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
     * @return
     */
    core::meta::Reference getMeta() const;

    /**
     *
     * @param fasta
     * @param fai
     */
    static void index(std::istream& fasta, std::ostream& fai);

    /**
     *
     * @param fasta
     * @param fai
     */
    static void hash(const FaiFile& fai, std::istream& fasta, std::ostream& hash);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_READER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
