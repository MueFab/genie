// Copyright 2018 The genie authors


/**
 *  @file FastaFileReader.h
 *  @brief FASTA file input interface
 *  @author Jan Voges
 *  @bug No known bugs
 */

#ifndef DSG_INPUT_FASTA_FASTAFILEREADER_H_
#define DSG_INPUT_FASTA_FASTAFILEREADER_H_


#include <string>
#include <vector>

#include "genie/constants.h"
#include "fileio/file_reader.h"
#include "fileio/fasta_record.h"


namespace dsg {
namespace input {
namespace fasta {


class FastaFileReader : public FileReader {
 public:
    FastaFileReader(
        const std::string& path);

    ~FastaFileReader(void);

    void parse(
        std::vector<FastaRecord> * const fastaRecords);
};


}  // namespace fasta
}  // namespace input
}  // namespace dsg


#endif  // DSG_INPUT_FASTA_FASTAFILEREADER_H_
