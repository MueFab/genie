/** @file CQFile.h
 *  @brief This file contains the definition of the CQFile class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_IO_CQ_CQFILE_H_
#define CALQ_IO_CQ_CQFILE_H_

#include <map>
#include <string>

#include "IO/File.h"
#include "QualCodec/Quantizers/Quantizer.h"

namespace calq {

class CQFile : public File {
 public:
    CQFile();
    CQFile(const std::string &path, const Mode &mode);
    ~CQFile(void);

    size_t nrReadFileFormatBytes(void) const;
    size_t nrWrittenFileFormatBytes(void) const;

    size_t readHeader(size_t *blockSize);
    size_t readQuantizers(std::map<int, Quantizer> *quantizers);
    size_t readQualBlock(std::string *block);

    size_t writeHeader(const size_t &blockSize);
    size_t writeQuantizers(const std::map<int, Quantizer> &quantizers);
    size_t writeQualBlock(unsigned char *block, const size_t &blockSize);

 private:
    static constexpr const char *MAGIC = "CQ";
    const size_t MAGIC_LEN = 3;

    size_t nrReadFileFormatBytes_;
    size_t nrWrittenFileFormatBytes_;
};

}  // namespace calq

#endif  // CALQ_IO_CQ_CQFILE_H_

