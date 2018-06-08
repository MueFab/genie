// Copyright 2018 The genie authors


/**
 *  @file FastqFileReader.cc
 *  @brief FASTQ file input implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "input/fastq/FastqFileReader.h"

#include <string.h>

#include <stdexcept>
#include <string>


namespace dsg {


FastqFileReader::FastqFileReader(
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


FastqFileReader::~FastqFileReader(void)
{
    free(m_line);
}


void FastqFileReader::parse(void)
{
    std::string currentHeader("");
    std::string currentSequence("");

    while (fgets(m_line, MAX_LINE_LENGTH, fp_) != NULL) {
        // Trim line
        size_t l = strlen(m_line) - 1;
        while (l && (m_line[l] == '\r' || m_line[l] == '\n')) {
            m_line[l--] = '\0';
        }

        if (m_line[0] == '@') {

    }

    FastqRecord currentFastqRecord(currentHeader, currentSequence);
    records.push_back(currentFastqRecord);
}


}  // namespace dsg

