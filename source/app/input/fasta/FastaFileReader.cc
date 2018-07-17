// Copyright 2018 The genie authors


/**
 *  @file FastaFileReader.cc
 *  @brief FASTA file input implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "FastaFileReader.h"

#include <string>

#include "common/exceptions.h"


namespace dsg {
namespace input {
namespace fasta {


FastaFileReader::FastaFileReader(
    const std::string& path)
    : FileReader(path)
{
    // Nothing to do here.
}


FastaFileReader::~FastaFileReader(void)
{
    // Nothing to do here.
}


void FastaFileReader::parse(
    std::vector<FastaRecord> * const fastaRecords)
{
    // Reset file pointer to the beginning of the file.
    size_t fpos = tell();
    seekFromSet(0);

    std::string currentHeader("");
    std::string currentSequence("");

    while (true) {
        // Read a line.
        std::string line("");
        readLine(&line);
        if (line.empty() == true) {
            break;
        }

        // Process line.
        if (line[0] == '>') {
            // Store the previous FASTA record, if there is one.
            if (currentSequence.empty() == false) {
                // We have a sequence, check if we have a header.
                if (currentHeader.empty() == true) {
                    throwRuntimeError("found sequence but no header");
                }

                FastaRecord currentFastaRecord(currentHeader, currentSequence);
                fastaRecords->push_back(currentFastaRecord);

                currentHeader = "";
                currentSequence = "";
            }

            // Store the header and trim it: remove everything after the first space.
            currentHeader = line;
            currentHeader = currentHeader.substr(0, currentHeader.find_first_of(" "));

        } else {
            currentSequence += line;
        }
    }

    FastaRecord currentFastaRecord(currentHeader, currentSequence);
    fastaRecords->push_back(currentFastaRecord);

    // Seek back.
    seekFromSet(fpos);
}


}  // namespace fasta
}  // namespace input
}  // namespace dsg

