// Copyright 2018 The genie authors


/**
 *  @file FastaRecord.h
 *  @brief FASTA record interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_INPUT_FASTA_FASTARECORD_H_
#define DSG_INPUT_FASTA_FASTARECORD_H_


#include <string>


namespace dsg {
namespace input {
namespace fasta {


struct FastaRecord {
 public:
    FastaRecord(
        const std::string& header,
        const std::string& sequence);

    ~FastaRecord(void);

 public:
    std::string header;
    std::string sequence;
};


}  // namespace fasta
}  // namespace input
}  // namespace dsg


#endif  // DSG_INPUT_FASTA_FASTARECORD_H_
