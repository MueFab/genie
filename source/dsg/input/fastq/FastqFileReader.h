// Copyright 2018 The genie authors


/**
 *  @file FastqFileReader.h
 *  @brief FASTQ file input interface
 *  @author Jan Voges
 *  @bug No known bugs
 */

#ifndef DSG_INPUT_FASTA_FASTQFILEREADER_H_
#define DSG_INPUT_FASTA_FASTQFILEREADER_H_


#include <vector>

#include "common/constants.h"
#include "input/FileReader.h"
#include "input/fasta/FastqRecord.h"


namespace dsg {


class FastqFileReader : public FileReader {
 public:

    FastqFileReader(
        const std::string& path);

    ~FastqFileReader(void);

 public:

     std::vector<FastqRecord> records;

 private:

     void parse(void);

 private:

    static const size_t MAX_LINE_LENGTH = sizeof(char) * (4 * KB);

    char *m_line;
};


}  // namespace dsg


#endif  // DSG_INPUT_FASTA_FASTQFILEREADER_H_

