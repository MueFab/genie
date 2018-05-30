// Copyright 2018 The genie authors


/**
 *  @file FASTAFile.cc
 *  @brief FASTA file implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */

#include "input/fasta/FASTAFile.h"

#include <string.h>

#include <stdexcept>
#include <utility>
#include <string>


namespace genie {
namespace dsg {


FASTAFile::FASTAFile(const std::string& path,
                     const Mode& mode)
    : File(path, mode),
      line_(NULL)
      {
    if (path.empty() == true) {
        throw std::runtime_error("path is empty");
    }

    if (mode != FASTAFile::MODE_READ) {
        throw std::runtime_error("Currently only MODE_READ supported");
    }

    // Usually, lines in a FASTA file should be limited to 80 chars, so 4 KB
    // should be enough
    line_ = reinterpret_cast<char *>(malloc(LINE_SIZE));
    if (line_ == NULL) {
        throw std::runtime_error("malloc failed");
    }

    // Parse the complete FASTA file
    parse();
}


FASTAFile::~FASTAFile(void)
{
    free(line_);
}


void FASTAFile::parse(void)
{
    std::string currentHeader("");
    std::string currentSequence("");

    while (fgets(line_, LINE_SIZE, fp_) != NULL) {
        // Trim line
        size_t l = strlen(line_) - 1;
        while (l && (line_[l] == '\r' || line_[l] == '\n')) {
            line_[l--] = '\0';
        }

        if (line_[0] == '>') {
            if (currentSequence.empty() == false) {
                // We have a sequence, check if we have a header
                if (currentHeader.empty() == true) {
                    throw std::runtime_error("Found sequence but no header");
                }

                // We have a header, check if it is already present in our
                // references map
                if (references.find(currentHeader) != references.end()) {
                    throw std::runtime_error("Found the same header twice");
                }

                // Everything ok, insert header-sequence pair into our
                // references map
                references.insert(std::pair<std::string, std::string>(currentHeader, currentSequence));
            }

            // Store the header and trim it: do not take the leading '>' and
            // remove everything after the first space
            currentHeader = line_ + 1;
            currentHeader = currentHeader.substr(0, currentHeader.find_first_of(" "));

            // Reset sequence
            currentSequence = "";
        } else {
            currentSequence += line_;
        }
    }

    references.insert(std::pair<std::string, std::string>(currentHeader, currentSequence));
}

}  // namespace dsg
}  // namespace genie
