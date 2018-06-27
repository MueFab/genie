// Copyright 2018 The genie authors


/**
 *  @file SamFileReader.cc
 *  @brief SAM file input implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */

#include "SamFileReader.h"

#include <iostream>
#include <string>

#include "input/sam/SamRecord.h"


namespace dsg {
namespace input {
namespace sam {


static void parseLine(
    const std::string& line,
    std::vector<std::string> * const fields)
{
    fields->clear();
    fields->push_back("");

    size_t position = 0;
    int fieldCount = 0;

    for (const auto& c : line) {
        if (c == '\t' && fieldCount <= 10) {
            fieldCount++;
            fields->push_back("");
        }

        (*fields)[fieldCount] += c;
    }
}


SamFileReader::SamFileReader(
    const std::string& path)
    : FileReader(path)
{
    readHeader();
}


SamFileReader::~SamFileReader(void)
{
    // Nothing to do here.
}


size_t SamFileReader::readRecords(
    const size_t numRecords,
    std::vector<SamRecord> * const records)
{
    std::string rnamePrev("");
    uint32_t posPrev = 0;

    for (size_t i = 0; i < numRecords; i++) {
        size_t fpos = tell();

        // Read a line.
        std::string line("");
        readLine(&line);
        if (line.empty() == true) {
            break;
        }

        // Parse line and construct samRecord
        std::vector<std::string> fields;
        parseLine(line, &fields);
        SamRecord samRecord(fields);

        records->push_back(samRecord);
    }

    return records->size();
}


void SamFileReader::readHeader(void)
{
    // Set file pointer to the beginning of the file.
    seekFromSet(0);

    size_t fpos = 0;

    while (true) {
        // Remember the file pointer position.
        fpos = tell();

        // Read a line.
        std::string line("");
        readLine(&line);

        // Add the line contents to the header.
        if (line[0] == '@') {
            header += line;
            header += "\n";
        } else {
            break;
        }
    }

    // Rewind to the beginning of the the alignment section.
    seekFromSet(fpos);

    if (header.empty() == true) {
        std::cout << "SAM header not present" << std::endl;
    }
}


}  // namespace sam
}  // namespace input
}  // namespace dsg

