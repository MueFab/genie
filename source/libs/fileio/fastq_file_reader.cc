// Copyright 2018 The genie authors

/**
 *  @file FastqFileReader.cc
 *  @brief FASTQ file input implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "fileio/fastq_file_reader.h"

#include <string>

#include "../apps/genie/exceptions.h"


namespace dsg {
namespace input {
namespace fastq {


FastqFileReader::FastqFileReader(
    const std::string& path)
    : FileReader(path)
{
    // Nothing to do here.
}


FastqFileReader::~FastqFileReader(void)
{
    // Nothing to do here.
}


size_t FastqFileReader::readRecords(
    const size_t numRecords,
    std::vector<FastqRecord> * const fastqRecords)
{
    fastqRecords->clear();

    while (true) {
        // Try to read 4 lines, i.e. an entire FASTQ record.
        FastqRecord fastqRecord;
        std::string line("");

        // Try to read the title line.
        readLine(&line);
        if (line.empty() == true) {
            return fastqRecords->size();
        }
        fastqRecord.title = line;

        readLine(&line);
        if (line.empty() == true) {
            throwRuntimeError("truncated FASTQ record");
        }
        fastqRecord.sequence = line;

        readLine(&line);
        if (line.empty() == true) {
            throwRuntimeError("truncated FASTQ record");
        }
        fastqRecord.optional = line;

        readLine(&line);
        if (line.empty() == true) {
            throwRuntimeError("truncated FASTQ record");
        }
        fastqRecord.qualityScores = line;

//         validateFastqRecords(fastqRecord);

        fastqRecords->push_back(fastqRecord);

        if (fastqRecords->size() == numRecords) {
            return fastqRecords->size();
        }
    }
}


}  // namespace fastq
}  // namespace input
}  // namespace dsg
