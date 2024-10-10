/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_SHA256FILE_H_
#define SRC_GENIE_FORMAT_FASTA_SHA256FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "picosha2/picosha2.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief
 */
class Sha256File {
    std::vector<std::pair<std::string, std::string>> data;
    /**
     * @brief
     * @param file
     * @return
     */
    static std::vector<std::pair<std::string, std::string>> read(std::istream& file);

 public:
    explicit Sha256File(std::istream& stream) { data = read(stream); }

    [[nodiscard]] const std::vector<std::pair<std::string, std::string>>& getData() const { return data; }

    /**
     * @brief
     * @param file
     * @param val
     */
    static void write(std::ostream& file, const std::vector<std::pair<std::string, std::string>>& val);

    /**
     * @brief
     * @param hex
     * @return
     */
    static std::string hex2bytes(const std::string& hex);

    /**
     * @brief
     * @param file
     * @param posStart
     * @param length
     * @return
     */
    static std::string hash(std::istream& file, size_t posStart, size_t length);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_SHA256FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
