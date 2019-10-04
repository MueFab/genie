#include "file.h"

#include <sys/stat.h>
#include <cassert>
#include <climits>

#include <string>

#include "util/exceptions.h"

namespace genie {

File::File(const std::string &path, const char *const mode) : m_fp(nullptr), m_path(path) {
    assert(!path.empty());
    assert(mode != nullptr);

    open(path, mode);
}

File::~File() { close(); }

void File::advance(off_t offset) { seekFromCur(offset); }

bool File::eof() const { return (feof(m_fp) != 0); }

FILE *File::handle() const { return m_fp; }

void File::seekFromCur(off_t offset) { seek(offset, SEEK_CUR); }

void File::seekFromEnd(off_t offset) { seek(offset, SEEK_END); }

void File::seekFromSet(off_t offset) { seek(offset, SEEK_SET); }

size_t File::size() {
    off_t tmp = tell();
    seekFromEnd(0);
    off_t fsize = tell();
    seekFromSet(tmp);
    return static_cast<size_t>(fsize);
}

off_t File::tell() const {
#ifdef _MSC_VER
    auto offset = static_cast<off_t>(ftell(m_fp));
#else
    off_t offset = ftello(m_fp);
#endif
    if (offset == -1) {
        UTILS_DIE("ftell failed on file: " + m_path);
    }
    return offset;
}

void File::close() {
    if (m_fp != nullptr) {
        int rc = fclose(m_fp);
        if (rc != 0) {
            UTILS_DIE("Failed to close file: " + m_path);
        }
        m_fp = nullptr;
    } else {
        UTILS_DIE("Failed to close file: " + m_path);
    }
}

void File::open(const std::string &path, const char *const mode) {
    assert(!path.empty());
    assert(mode != nullptr);
    assert(m_fp == nullptr);

#ifdef _MSC_VER
    int err = fopen_s(&m_fp, path.c_str(), mode);
    if (err != 0) {
        GABACIFY_DIE("Failed to open file: " + m_path);
    }
#else
    m_fp = fopen(path.c_str(), mode);
    if (m_fp == nullptr) {
        UTILS_DIE("Failed to open file: " + m_path);
    }
#endif

#ifdef _MSC_VER
    int fd = _fileno(m_fp);
#else
    int fd = fileno(m_fp);
#endif
    if (fd == -1) {
        UTILS_DIE("Failed to get file descriptor of file: " + m_path);
    }

    struct stat st {};
#ifdef _MSC_VER
    if ((fstat(fd, &st) != 0) || ((st.st_mode) & S_IFMT) != S_IFREG) {
        GABACIFY_DIE("Not a regular file: " + m_path);
    }
#else
    if ((fstat(fd, &st) != 0) || (!S_ISREG(st.st_mode))) {
        UTILS_DIE("Not a regular file: " + m_path);
    }
#endif
}

void File::seek(off_t offset, int whence) {
#ifdef _MSC_VER
    int rc = fseek(m_fp, offset, whence);
#else
    int rc = fseeko(m_fp, offset, whence);
#endif
    if (rc != 0) {
        UTILS_DIE("fseek failed on file: " + m_path);
    }
}

}  // namespace genie
