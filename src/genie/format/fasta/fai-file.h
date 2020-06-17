/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FAI_FILE_H
#define GENIE_FAI_FILE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <set>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 *
 */
class FaiFile {
   public:
    /**
     *
     */
    struct FaiSequence {
        std::string name;  //!<
        uint64_t length;   //!<
        uint64_t offset;   //!<
        size_t linebases;  //!<
        size_t linewidth;  //!<

        /**
         *
         * @param stream
         */
        explicit FaiSequence(std::istream& stream);

        /**
         *
         */
        FaiSequence();
    };
    /**
     *
     * @param seq
     */
    void addSequence(const FaiSequence& seq);

    /**
     *
     * @param stream
     */
    explicit FaiFile(std::istream& stream);

    /**
     *
     */
    FaiFile() = default;

    /**
     *
     * @param sequence
     * @param position
     * @return
     */
    uint64_t getFilePosition(const std::string& sequence, uint64_t position) const;

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
    bool containsSequence(const std::string& seq) const;

    /**
     *
     * @param seq
     * @return
     */
    uint64_t getLength(const std::string& seq) const;

   private:
    std::map<std::string, FaiSequence> seqs;  //!<

    friend std::ostream& operator<<(std::ostream& stream, const FaiFile& file);
};

/**
 *
 * @param stream
 * @param file
 * @return
 */
std::ostream& operator<<(std::ostream& stream, const FaiFile::FaiSequence& file);

/**
 *
 * @param stream
 * @param file
 * @return
 */
std::ostream& operator<<(std::ostream& stream, const FaiFile& file);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FAI_FILE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------