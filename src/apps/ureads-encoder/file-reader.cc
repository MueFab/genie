#include "file-reader.h"

#include <climits>
#include <cstring>

#include "util/exceptions.h"

namespace genie {

FileReader::FileReader() : m_fp(nullptr), m_fsize(0) {}

FileReader::FileReader(const std::string& path) : m_fp(nullptr), m_fsize(0) {
    if (path.empty()) {
        UTILS_DIE("path is empty");
    }

    open(path);

    // Usually, lines in a FASTA file should be limited to 80 chars, so 4 KB should be enough.
    m_line = reinterpret_cast<char*>(malloc(MAX_LINE_LENGTH));
    if (m_line == nullptr) {
        UTILS_DIE("malloc failed");
    }
}

FileReader::~FileReader() {
    free(m_line);
    close();
}

void FileReader::open(const std::string& path) {
    if (path.empty()) {
        UTILS_DIE("path is empty");
    }

    if (m_fp != nullptr) {
        UTILS_DIE("file pointer already in use");
    }

    const char* mode = "rb";

#ifdef _WIN32
    int rc = fopen_s(&m_fp, path.c_str(), mode);
    if (rc != 0) {
        UTILS_DIE("failed to open file");
    }
#else
    m_fp = fopen(path.c_str(), mode);
    if (m_fp == nullptr) {
        UTILS_DIE("failed to open file");
    }
#endif

    // Compute file size.
    seekFromEnd(0);
    m_fsize = tell();
    seekFromSet(0);
}

void FileReader::close() {
    if (m_fp != nullptr) {
        fclose(m_fp);
        m_fp = nullptr;
    } else {
        UTILS_DIE("failed to close file");
    }
}

// void FileReader::advance(const int64_t offset)
//{
//    seekFromCur(offset);
//}

bool FileReader::eof() const { return feof(m_fp) != 0; }

void* FileReader::handle() const { return m_fp; }

void FileReader::readLine(std::string* const line) {
    line->clear();

    char* rc = fgets(m_line, MAX_LINE_LENGTH, m_fp);

    if (rc == nullptr) {
        // This means:
        //   - error during read
        //   OR
        //   - encountered EOF, nothing has been read
        // We just return to the caller and leave 'line' empty.
        return;
    }

    if (eof()) {
        // This means: EOF was reached but contents were read into 'm_line'
        // and 'rc', respectively.
        // We just proceed processing the read contents.
    }

    // Trim line.
    size_t l = strlen(m_line) - 1;
    while (l && ((m_line[l] == '\r') || (m_line[l] == '\n'))) {
        m_line[l--] = '\0';
    }

    *line = m_line;
}

// void FileReader::seekFromCur(const int64_t offset)
//{
//    seek(offset, SEEK_CUR);
//}

void FileReader::seekFromEnd(const int64_t offset) { seek(offset, SEEK_END); }

void FileReader::seekFromSet(const int64_t offset) { seek(offset, SEEK_SET); }

int64_t FileReader::size() const { return m_fsize; }

int64_t FileReader::tell() const {
    auto offset = static_cast<int64_t>(ftell(m_fp));

    if (offset == -1) {
        UTILS_DIE("ftell failed");
    }

    return offset;
}

void FileReader::seek(const int64_t offset, const int whence) {
    if (offset > LONG_MAX) {
        UTILS_DIE("position out of range");
    }

    int rc = fseek(m_fp, offset, whence);
    if (rc != 0) {
        UTILS_DIE("fseek failed");
    }
}

}  // namespace genie
