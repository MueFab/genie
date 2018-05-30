// Copyright 2018 The genie authors


/**
 *  @file FASTAFile.h
 *  @brief FASTA file interface
 *  @author Jan Voges
 *  @bug No known bugs
 */

#ifndef DSG_INPUT_FASTA_FASTAFILE_H_
#define DSG_INPUT_FASTA_FASTAFILE_H_


#include <map>
#include <string>

#include "common/constants.h"
#include "input/File.h"


namespace dsg {


class FASTAFile : public File {
 public:

    FastaFile(
        const std::string& path,
        const Mode& mode = MODE_READ);

    ~FASTAFile(void);

 public:

     FastaIndex index;

 private:

     void constructIndex(void);

 private:

    static const size_t MAX_LINE_LENGTH = sizeof(char) * (4 * KB);

    char *m_line;
};


}  // namespace dsg


#endif  // DSG_INPUT_FASTA_FASTAFILE_H_
