// Copyright 2018 The genie authors


/**
 *  @file FastqRecord.h
 *  @brief FASTQ record interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_INPUT_FASTQ_FASTQRECORD_H_
#define DSG_INPUT_FASTQ_FASTQRECORD_H_


#include <string>


namespace dsg {
namespace input {
namespace fastq {


struct FastqRecord {
 public:
    FastqRecord(void);

    FastqRecord(
        const std::string& title,
        const std::string& sequence,
        const std::string& optional,
        const std::string& qualityScores);

    ~FastqRecord(void);

 public:
    std::string title;
    std::string sequence;
    std::string optional;
    std::string qualityScores;
};


}  // namespace fastq
}  // namespace input
}  // namespace dsg


#endif  // DSG_INPUT_FASTQ_FASTQRECORD_H_

