/** @file File.cc
 *  @brief This file contains the implementation of the File class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "IO/File.h"

#include <limits.h>
#include <errno.h>
#include "Common/Exceptions.h"
#include "Common/os.h"

namespace calq {

File::File(void)
    : fp_(NULL),
      fsize_(0),
      isOpen_(false),
      mode_(File::MODE_READ),
      nrReadBytes_(0),
      nrWrittenBytes_(0) {}

File::File(const std::string &path, const Mode mode)
    : fp_(NULL),
      fsize_(0),
      isOpen_(false),
      mode_(mode),
      nrReadBytes_(0),
      nrWrittenBytes_(0) {
    if (path.empty() == true) {
        throwErrorException("path is empty");
    }

    open(path, mode);
}

File::~File(void) {
    close();
}

void File::open(const std::string &path, const Mode mode) {

    if (path.empty() == true) {
        throwErrorException("path is empty");
    }
    if (fp_ != NULL) {

        throwErrorException("File pointer already in use");
    }

    const char *m;
    if (mode == MODE_READ) {
        m = "rb";
        mode_ = mode;
    } else if (mode == MODE_WRITE) {
        m = "wb";
        mode_ = mode;
    } else {

        throwErrorException("Unkown mode");
    }

    printf("%s", path.c_str());

#ifdef CQ_OS_WINDOWS
    int err = fopen_s(&fp_, path.c_str(), m);
    if (err != 0) {
        throwErrorException("Failed to open file");
    }
#else
    errno = 0;
    fp_ = fopen(path.c_str(), m);
    if (fp_ == NULL) {
        printf("Failed to open file (Relative path to given folder is needed)");
        throwErrorException("Failed to open file");
    }
#endif

    // Compute file size
    fseek(fp_, 0, SEEK_END);
    fsize_ = ftell(fp_);
    fseek(fp_, 0, SEEK_SET);

    isOpen_ = true;
}

void File::close(void) {
    if (isOpen_ == true) {
        if (fp_ != NULL) {
            fclose(fp_);
            fp_ = NULL;
        } else {
            throwErrorException("Failed to close file");
        }
    }
}

void File::advance(const size_t offset) {
    int ret = fseek(fp_, (long int)offset, SEEK_CUR);
    if (ret != 0) {
        throwErrorException("fseek failed");
    }
}

bool File::eof(void) const {
    int eof = feof(fp_);
    return eof != 0 ? true : false;
}

void * File::handle(void) const {
    return fp_;
}

void File::seek(const size_t pos) {
    if (pos > LONG_MAX) {
        throwErrorException("pos out of range");
    }
    int ret = fseek(fp_, (long)pos, SEEK_SET);
    if (ret != 0) {
        throwErrorException("fseek failed");
    }
}

size_t File::size(void) const {
    return fsize_;
}

size_t File::tell(void) const {
    long int offset = ftell(fp_);
    if (offset == -1) {
        throwErrorException("ftell failed");
    }
    return offset;
}

size_t File::nrReadBytes(void) const {
    if (mode_ != MODE_READ) {
        throwErrorException("File is not open in read mode");
    }
    return nrReadBytes_;
}

size_t File::nrWrittenBytes(void) const {
    if (mode_ != MODE_WRITE) {
        throwErrorException("File is not open in write mode");
    }
    return nrWrittenBytes_;
}

bool File::isReadable(void) const {
    if (isOpen_ == true && mode_ == MODE_READ)
        return true;
    return false;
}

bool File::isWritable(void) const {
    if (isOpen_ == true && mode_ == MODE_WRITE)
        return true;
    return false;
}

size_t File::read(void *buffer, const size_t size) {
    if (buffer == NULL) {
        throwErrorException("buffer is NULL");
    }
    if (size == 0) {
        return 0;
    }
    size_t ret = fread(buffer, 1, size, fp_);
    if (ret != size) {
        throwErrorException("fread failed");
    }
    nrReadBytes_ += ret;
    return ret;
}

size_t File::write(void *buffer, const size_t size) {
    if (buffer == NULL) {
        throwErrorException("buffer is NULL");
    }
    if (size == 0) {
        return 0;
    }
    size_t ret = fwrite(buffer, 1, size, fp_);
    if (ret != size) {
        throwErrorException("fwrite failed");
    }
    nrWrittenBytes_ += ret;
    return ret;
}

size_t File::readByte(unsigned char *byte) {
    size_t ret = fread(byte, 1, 1, fp_);
    if (ret != sizeof(unsigned char)) {
        throwErrorException("fread failed");
    }
    nrReadBytes_++;
    return ret;
}

size_t File::readUint8(uint8_t *byte) {
    return readByte(byte);
}

size_t File::readUint16(uint16_t *word) {
    unsigned char *buffer = (unsigned char *)malloc(sizeof(uint16_t));
    if (buffer == NULL) {
        throwErrorException("malloc failed");
    }

    size_t ret = read(buffer, sizeof(uint16_t));

    if (ret != sizeof(uint16_t)) {
        free(buffer);
        throwErrorException("read failed");
    } else {
        *word = (uint16_t)buffer[0] <<  8 | (uint16_t)buffer[1];
        free(buffer);
    }

    return ret;
}

size_t File::readUint32(uint32_t *dword) {
    unsigned char *buffer = (unsigned char *)malloc(sizeof(uint32_t));
    if (buffer == NULL) {
        throwErrorException("malloc failed");
    }

    size_t ret = read(buffer, sizeof(uint32_t));

    if (ret != sizeof(uint32_t)) {
        free(buffer);
        throwErrorException("read failed");
    } else {
        *dword = (uint32_t)buffer[0] << 24 |
                 (uint32_t)buffer[1] << 16 |
                 (uint32_t)buffer[2] <<  8 |
                 (uint32_t)buffer[3];
        free(buffer);
    }

    return ret;
}

size_t File::readUint64(uint64_t *qword) {
    unsigned char *buffer = (unsigned char *)malloc(sizeof(uint64_t));
    if (buffer == NULL) {
        throwErrorException("malloc failed");
    }

    size_t ret = read(buffer, sizeof(uint64_t));

    if (ret != sizeof(uint64_t)) {
        free(buffer);
        throwErrorException("read failed");
    } else {
        *qword = (uint64_t)buffer[0] << 56 |
                 (uint64_t)buffer[1] << 48 |
                 (uint64_t)buffer[2] << 40 |
                 (uint64_t)buffer[3] << 32 |
                 (uint64_t)buffer[4] << 24 |
                 (uint64_t)buffer[5] << 16 |
                 (uint64_t)buffer[6] <<  8 |
                 (uint64_t)buffer[7];
        free(buffer);
    }

    return ret;
}

size_t File::writeByte(const unsigned char byte) {
    size_t ret = fwrite(&byte, 1, 1, fp_);
    if (ret != sizeof(unsigned char)) {
        throwErrorException("fwrite failed");
    }
    nrWrittenBytes_++;
    return ret;
}

size_t File::writeUint8(const uint8_t byte) {
    return writeByte(byte);
}

size_t File::writeUint16(const uint16_t word) {
    size_t ret = 0;
    ret += writeByte((unsigned char)(word >> 8) & 0xFF);
    ret += writeByte((unsigned char)(word)      & 0xFF);
    return ret;
}

size_t File::writeUint32(const uint32_t dword) {
    size_t ret = 0;
    ret += writeByte((unsigned char)(dword >> 24) & 0xFF);
    ret += writeByte((unsigned char)(dword >> 16) & 0xFF);
    ret += writeByte((unsigned char)(dword >>  8) & 0xFF);
    ret += writeByte((unsigned char)(dword)       & 0xFF);
    return ret;
}

size_t File::writeUint64(const uint64_t qword) {
    size_t ret = 0;
    ret += writeByte((unsigned char)(qword >> 56) & 0xFF);
    ret += writeByte((unsigned char)(qword >> 48) & 0xFF);
    ret += writeByte((unsigned char)(qword >> 40) & 0xFF);
    ret += writeByte((unsigned char)(qword >> 32) & 0xFF);
    ret += writeByte((unsigned char)(qword >> 24) & 0xFF);
    ret += writeByte((unsigned char)(qword >> 16) & 0xFF);
    ret += writeByte((unsigned char)(qword >>  8) & 0xFF);
    ret += writeByte((unsigned char)(qword)       & 0xFF);
    return ret;
}

}  // namespace calq

