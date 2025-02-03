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
#include "genie/util/make_unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 * @brief
 */
class FastaReader {
 private:
    Sha256File hashFile;  //!< @brief
    FaiFile fai;          //!< @brief
    std::istream* fasta;  //!< @brief
    std::string path;     //!< @brief

 public:
    /**
     * @brief
     * @param fastaFile
     * @param faiFile
     * @param sha256File
     * @param _path
     */
    FastaReader(std::istream& fastaFile, std::istream& faiFile, std::istream& sha256File, std::string _path);

    /**
     * @brief
     * @return
     */
    std::map<size_t, std::string> getSequences() const;

    /**
     * @brief
     * @param name
     * @return
     */
    uint64_t getLength(const std::string& name) const;

    /**
     * @brief
     * @param sequence
     * @param start
     * @param end
     * @return
     */
    std::string loadSection(const std::string& sequence, uint64_t start, uint64_t end);

    /**
     * @brief
     * @return
     */
    core::meta::Reference getMeta() const;

    /**
     * @brief
     * @param fasta
     * @param fai
     */
    static void index(std::istream& fasta, std::ostream& fai);

    /**
     * @brief
     * @param fasta
     * @param fai
     * @param hash
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
