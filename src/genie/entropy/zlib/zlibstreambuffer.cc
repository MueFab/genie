/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/zlib/zlibstreambuffer.h"

#include <zlib.h>
#include <iostream>
#include <streambuf>

namespace genie::entropy::zlib {

ZlibStreamBuffer::ZlibStreamBuffer(const std::string& file_path, bool mode) {
    if (mode) {
        file = gzopen(file_path.c_str(), "wb");
        std::cout << "Opening file " << file_path << std::endl;
    } else {
        file = gzopen(file_path.c_str(), "rb");
        std::cout << "Opening file " << file_path << std::endl;
        char c;
        gzread(file, &c, 1);
        this->buffer = c;
    }
}

ZlibStreamBuffer::~ZlibStreamBuffer() { gzclose(file); }

std::streamsize ZlibStreamBuffer::showmanyc() { return file->have; }

std::streamsize ZlibStreamBuffer::xsgetn(char* s, std::streamsize n) {
    if (this->buffer.has_value()) {
        *s = this->buffer.value();
        s++;
        int readChars = 1;
        int err;
        for (int i = 0; i < n; i++) {
            err = gzread(file, s, 1);
            readChars += err;
            if (gzeof(file)) {
                break;
            }
            s++;
        }
        gzread(file, &this->buffer.value(), 1);
        return readChars;
    }
    return 0;
}

std::streamsize ZlibStreamBuffer::xsputn(const char* s, std::streamsize n) {
    if (this->buffer.has_value()) {
        int writeChars = gzwrite(file, &this->buffer.value(), 1);
        for (int i = 0; i < n; i++) {
            writeChars += gzwrite(file, s, 1);
            s++;
            if (gzeof(file)) {
                return writeChars;
            }
        }
        this->buffer.value() = *s;
        return writeChars;
    }

    int writeChars = 0;
    for (int i = 0; i < n; i++) {
        writeChars += gzwrite(file, s, 1);
        s++;
        if (gzeof(file)) {
            return writeChars;
        }
    }
    return writeChars;
}

int ZlibStreamBuffer::overflow(int c) {
    if (c == EOF) {
        this->buffer = std::nullopt;
        return EOF;
    }
    this->buffer.value() = c;
    return c;
}

int ZlibStreamBuffer::underflow() {
    if (this->buffer.has_value()) {
        return this->buffer.value();
    }
    return EOF;
}

int ZlibStreamBuffer::uflow() {
    if (this->buffer.has_value()) {
        char local_buffer = this->buffer.value();
        gzread(file, &this->buffer, 1);
        if (gzeof(file)) {
            this->buffer = std::nullopt;
        }
        return local_buffer;
    }
    return EOF;
}

}  // namespace genie::entropy::zlib
