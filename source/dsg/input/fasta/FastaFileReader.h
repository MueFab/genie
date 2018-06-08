// Copyright 2018 The genie authors


/**
 *  @file FastaFileReader.h
 *  @brief FASTA file input interface
 *  @author Jan Voges
 *  @bug No known bugs
 */

#ifndef DSG_INPUT_FASTA_FASTAFILEREADER_H_
#define DSG_INPUT_FASTA_FASTAFILEREADER_H_


#include <vector>

#include "common/constants.h"
#include "input/FileReader.h"
#include "input/fasta/FastaRecord.h"


namespace dsg {


class FastaFileReader : public FileReader {
 public:

    FastaFileReader(
        const std::string& path);

    ~FastaFileReader(void);

 public:

     std::vector<FastaRecord> records;

 private:

     void parse(void);

 private:

    static const size_t MAX_LINE_LENGTH = sizeof(char) * (4 * KB);

    char *m_line;
};


}  // namespace dsg


#endif  // DSG_INPUT_FASTA_FASTAFILEREADER_H_
