/** @file FASTAFile.h
 *  @brief This file contains the definition of the FASTAFile class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_IO_FASTA_FASTAFILE_H_
#define CALQ_IO_FASTA_FASTAFILE_H_

#include <map>
#include <string>

#include "Common/constants.h"
#include "IO/File.h"

namespace calq {

class FASTAFile : public File {
 public:
    explicit FASTAFile(const std::string &path, const Mode &mode = MODE_READ);
    ~FASTAFile(void);

    std::map<std::string, std::string> references;

 private:
    static const size_t LINE_SIZE = sizeof(char) * (4*KB);

    void parse(void);

    char *line_;
};

}  // namespace calq

#endif  // CALQ_IO_FASTA_FASTAFILE_H_
