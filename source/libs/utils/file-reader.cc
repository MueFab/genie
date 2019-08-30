#include "file-reader.h"

#include <limits.h>
#include <string.h>

#include <stdexcept>

namespace utils {

FileReader::FileReader() : fp_(nullptr), fsize_(0), line_(nullptr) {}

FileReader::FileReader(const std::string &path) : fp_(nullptr), fsize_(0), line_(nullptr) {
    if (path.empty()) {
        throw std::runtime_error{"Tried to open void path"};
    }

    open(path);

    line_ = reinterpret_cast<char *>(malloc(MAX_LINE_LENGTH));
    if (line_ == nullptr) {
        throw std::runtime_error{"Failed to allocate memory"};
    }
}

FileReader::~FileReader() {
    free(line_);
    close();
}

void FileReader::open(const std::string &path) {
    if (path.empty()) {
        throw std::runtime_error{"Tried to open void path"};
    }

    if (fp_ != nullptr) {
        throw std::runtime_error{"File pointer already in use"};
    }

    const char *mode = "rb";

#ifdef _WIN32
    int rc = fopen_s(&fp_, path.c_str(), mode);
    if (rc != 0) {
        throw std::runtime_error{"Failed to open file"};
    }
#else
    fp_ = fopen(path.c_str(), mode);
    if (fp_ == nullptr) {
        throw std::runtime_error{"Failed to open file"};
    }
#endif

    // Compute file size
    seekFromEnd(0);
    fsize_ = tell();
    seekFromSet(0);
}

void FileReader::close() {
    if (fp_ != nullptr) {
        fclose(fp_);
        fp_ = nullptr;
    } else {
        throw std::runtime_error{"Failed to close file"};
    }
}

void FileReader::advance(const int64_t offset) { seekFromCur(offset); }

bool FileReader::eof() const { return feof(fp_) != 0; }

void *FileReader::handle() const { return fp_; }

void FileReader::readLine(std::string *const line) {
    line->clear();

    char *rc = fgets(line_, MAX_LINE_LENGTH, fp_);

    if (rc == nullptr) {
        // Error during read OR encountered EOF. Nothing has been read. We just return to the caller and leave
        // 'line' empty.
        return;
    }

    if (eof()) {
        // EOF was reached but contents were read into 'line_'. Proceed processing the read contents.
    }

    // Trim line
    size_t l = strlen(line_) - 1;
    while (l && ((line_[l] == '\r') || (line_[l] == '\n'))) {
        line_[l--] = '\0';
    }

    *line = line_;
}

void FileReader::seekFromCur(const int64_t offset) { seek(offset, SEEK_CUR); }

void FileReader::seekFromEnd(const int64_t offset) { seek(offset, SEEK_END); }

void FileReader::seekFromSet(const int64_t offset) { seek(offset, SEEK_SET); }

size_t FileReader::size() const { return fsize_; }

int64_t FileReader::tell() const {
    auto offset = static_cast<int64_t>(ftell(fp_));

    if (offset == -1) {
        throw std::runtime_error{"Failed to get file pointer position"};
    }

    return offset;
}

void FileReader::seek(const int64_t offset, const int whence) {
    if (offset > LONG_MAX) {
        throw std::runtime_error{"Position out of range"};
    }

    int rc = fseek(fp_, offset, whence);
    if (rc != 0) {
        throw std::runtime_error{"Failed to get file pointer position"};
    }
}

}  // namespace utils
