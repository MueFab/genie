/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_FAI_FILE_H_
#define SRC_GENIE_FORMAT_FASTA_FAI_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <map>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 * @brief
 */
class FaiFile {
 public:
    /**
     * @brief
     */
    struct FaiSequence {
        std::string name;  //!< @brief
        uint64_t length;   //!< @brief
        uint64_t offset;   //!< @brief
        size_t linebases;  //!< @brief
        size_t linewidth;  //!< @brief

        /**
         * @brief
         * @param stream
         */
        explicit FaiSequence(std::istream& stream);

        /**
         * @brief
         */
        FaiSequence();
    };
    /**
     * @brief
     * @param seq
     */
    void addSequence(const FaiSequence& seq);

    /**
     * @brief
     * @param stream
     */
    explicit FaiFile(std::istream& stream);

    /**
     * @brief
     */
    FaiFile() = default;

    /**
     * @brief
     * @param sequence
     * @param position
     * @return
     */
    uint64_t getFilePosition(const std::string& sequence, uint64_t position) const;

    /**
     * @brief
     * @return
     */
    std::map<size_t, std::string> getSequences() const;

    /**
     * @brief
     * @param seq
     * @return
     */
    bool containsSequence(const std::string& seq) const;

    /**
     * @brief
     * @param seq
     * @return
     */
    uint64_t getLength(const std::string& seq) const;

 private:
    std::map<std::string, FaiSequence> seqs;  //!< @brief
    std::map<size_t, std::string> indices; //!< @brief

    /**
     * @brief
     * @param stream
     * @param file
     * @return
     */
    friend std::ostream& operator<<(std::ostream& stream, const FaiFile& file);
};

/**
 * @brief
 * @param stream
 * @param file
 * @return
 */
std::ostream& operator<<(std::ostream& stream, const FaiFile::FaiSequence& file);

/**
 * @brief
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

#endif  // SRC_GENIE_FORMAT_FASTA_FAI_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
