// Copyright 2018 The genie authors


/**
 *  @file FastaFileReader.cc
 *  @brief FASTA file input implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "input/fasta/FastaFileReader.h"

#include <string.h>

#include <stdexcept>
#include <string>


namespace dsg {


FastaFileReader::FastaFileReader(
    const std::string& path)
    : FileReader(path),
      records(),
      m_line(NULL)
{
    if (path.empty() == true) {
        throw std::runtime_error("path is empty");
    }

    // Usually, lines in a FASTA file should be limited to 80 chars, so 4 KB
    // should be enough
    m_line = reinterpret_cast<char *>(malloc(MAX_LINE_LENGTH));
    if (m_line == NULL) {
        throw std::runtime_error("malloc failed");
    }

    // Parse the complete FASTA file
    parse();
}


FastaFileReader::~FastaFileReader(void)
{
    free(m_line);
}


void FastaFileReader::parse(void)
{
    std::string currentHeader("");
    std::string currentSequence("");

    while (fgets(m_line, MAX_LINE_LENGTH, fp_) != NULL) {
        // Trim line
        size_t l = strlen(m_line) - 1;
        while (l && (m_line[l] == '\r' || m_line[l] == '\n')) {
            m_line[l--] = '\0';
        }

        if (m_line[0] == '>') {
            if (currentSequence.empty() == false) {
                // We have a sequence, check if we have a header
                if (currentHeader.empty() == true) {
                    throw std::runtime_error("found sequence but no header");
                }

                FastaRecord currentFastaRecord(currentHeader, currentSequence);
                records.push_back(currentFastaRecord);
            }

            // Store the header and trim it: do not take the leading '>' and
            // remove everything after the first space
            currentHeader = m_line + 1;
            currentHeader = currentHeader.substr(0, currentHeader.find_first_of(" "));

            // Reset sequence
            currentSequence = "";
        } else {
            currentSequence += m_line;
        }
    }

    FastaRecord currentFastaRecord(currentHeader, currentSequence);
    records.push_back(currentFastaRecord);
}


}  // namespace dsg
