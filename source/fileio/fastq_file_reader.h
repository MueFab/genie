// Copyright 2018 The genie authors


/**
 *  @file FastqFileReader.h
 *  @brief FASTQ file input interface
 *  @author Jan Voges
 *  @bug No known bugs
 */

#ifndef DSG_INPUT_FASTQ_FASTQFILEREADER_H_
#define DSG_INPUT_FASTQ_FASTQFILEREADER_H_


#include <string>
#include <vector>

#include "genie/constants.h"
#include "fileio/file_reader.h"
#include "fileio/fastq_record.h"


namespace dsg {
namespace input {
namespace fastq {


class FastqFileReader : public FileReader {
 public:
    FastqFileReader(
        const std::string& path);

    ~FastqFileReader(void);

    size_t readRecords(
        const size_t numRecords,
        std::vector<FastqRecord> * const records);
};


}  // namespace fastq
}  // namespace input
}  // namespace dsg


#endif  // DSG_INPUT_FASTQ_FASTQFILEREADER_H_
