// Copyright 2018 The genie authors

/**
 *  @file FileReader.h
 *  @brief File input implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */

#include "input/FileReader.h"

#include <limits.h>
#include <stdexcept>

#include "common/os.h"


namespace dsg {


FileReader::FileReader(void)
    : fp_(NULL),
      fsize_(0),
      isOpen_(false)
{
    // Nothing to do here.
}


FileReader::FileReader(
    const std::string &path)
    : fp_(NULL),
      fsize_(0),
      isOpen_(false)
{
    if (path.empty() == true) {
        throw std::runtime_error("path is empty");
    }

    open(path);
}


FileReader::~FileReader(void)
{
    close();
}


void FileReader::open(
    const std::string& path)
{
    if (path.empty() == true) {
        throw std::runtime_error("path is empty");
    }

    if (fp_ != NULL) {
        throw std::runtime_error("File pointer already in use");
    }

    const char *mode = "rb";

#ifdef CQ_OS_WINDOWS
    int err = fopen_s(&fp_, path.c_str(), mode);
    if (err != 0) {
        throw std::runtime_error("failed to open file");
    }
#else
    fp_ = fopen(path.c_str(), mode);
    if (fp_ == NULL) {
        throw std::runtime_error("failed to open file");
    }
#endif

    // Compute file size.
    fseek(fp_, 0, SEEK_END);
    fsize_ = ftell(fp_);
    fseek(fp_, 0, SEEK_SET);

    isOpen_ = true;
}


void FileReader::close(void)
{
    if (isOpen_ == true) {
        if (fp_ != NULL) {
            fclose(fp_);
            fp_ = NULL;
        } else {
            throw std::runtime_error("failed to close file");
        }
    }
}


void FileReader::advance(
    const size_t offset)
{
    int ret = fseek(fp_, (long int)offset, SEEK_CUR);
    if (ret != 0) {
        throw std::runtime_error("fseek failed");
    }
}


bool FileReader::eof(void) const
{
    return feof(fp_) != 0 ? true : false;
}


void * FileReader::handle(void) const
{
    return fp_;
}


void FileReader::seek(
    const size_t pos)
{
    if (pos > LONG_MAX) {
        throw std::runtime_error("pos out of range");
    }

    int ret = fseek(fp_, (long)pos, SEEK_SET);
    if (ret != 0) {
        throw std::runtime_error("fseek failed");
    }
}


size_t FileReader::size(void) const
{
    return fsize_;
}


size_t FileReader::tell(void) const
{
    long int offset = ftell(fp_);

    if (offset == -1) {
        throw std::runtime_error("ftell failed");
    }

    return offset;
}


}  // namespace dsg
