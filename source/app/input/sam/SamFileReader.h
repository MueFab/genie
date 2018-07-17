// Copyright 2018 The genie authors


/**
 *  @file SamFileReader.h
 *  @brief SAM file input interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_INPUT_SAM_SAMFILEREADER_H_
#define DSG_INPUT_SAM_SAMFILEREADER_H_


#include <string>
#include <vector>

#include "input/FileReader.h"
#include "input/sam/SamRecord.h"


namespace dsg {
namespace input {
namespace sam {


class SamFileReader : public FileReader {
 public:
    SamFileReader(
        const std::string& path);

    ~SamFileReader(void);

    size_t readRecords(
        const size_t numRecords,
        std::vector<SamRecord> * const records);

 public:
    std::string header;

 private:
    void readHeader(void);
};


}  // namespace sam
}  // namespace input
}  // namespace dsg


#endif  // DSG_INPUT_SAM_SAMFILEREADER_H_

