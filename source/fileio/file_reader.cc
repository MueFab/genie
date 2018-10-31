// Copyright 2018 The genie authors

/**
 *  @file FileReader.h
 *  @brief File input implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */

#include "fileio/file_reader.h"

#include <limits.h>
#include <string.h>

#include "genie/exceptions.h"
#include "genie/operating-system.h"


namespace dsg {
namespace input {


FileReader::FileReader(void)
    : m_fp(NULL),
      m_fsize(0)
{
    // Nothing to do here.
}


FileReader::FileReader(
    const std::string& path)
    : m_fp(NULL),
      m_fsize(0)
{
    if (path.empty() == true) {
        throwRuntimeError("path is empty");
    }

    open(path);

    // Usually, lines in a FASTA file should be limited to 80 chars, so 4 KB
    // should be enough.
    m_line = reinterpret_cast<char *>(malloc(MAX_LINE_LENGTH));
    if (m_line == NULL) {
        throwRuntimeError("malloc failed");
    }
}


FileReader::~FileReader(void)
{
    free(m_line);

    close();
}


void FileReader::open(
    const std::string& path)
{
    if (path.empty() == true) {
        throwRuntimeError("path is empty");
    }

    if (m_fp != NULL) {
        throwRuntimeError("file pointer already in use");
    }

    const char *mode = "rb";

#ifdef OS_WINDOWS
    int rc = fopen_s(&m_fp, path.c_str(), mode);
    if (rc != 0) {
        throwRuntimeError("failed to open file");
    }
#else
    m_fp = fopen(path.c_str(), mode);
    if (m_fp == NULL) {
        throwRuntimeError("failed to open file");
    }
#endif

    // Compute file size.
    seekFromEnd(0);
    m_fsize = tell();
    seekFromSet(0);
}


void FileReader::close(void)
{
    if (m_fp != NULL) {
        fclose(m_fp);
        m_fp = NULL;
    } else {
        throwRuntimeError("failed to close file");
    }
}


void FileReader::advance(
    const int64_t offset)
{
    seekFromCur(offset);
}


bool FileReader::eof(void) const
{
    return (feof(m_fp) != 0) ? true : false;
}


void * FileReader::handle(void) const
{
    return m_fp;
}


void FileReader::readLine(
    std::string * const line)
{
    line->clear();

    char *rc = fgets(m_line, MAX_LINE_LENGTH, m_fp);

    if (rc == NULL) {
        // This means:
        //   - error during read
        //   OR
        //   - encountered EOF, nothing has been read
        // We just return to the caller and leave 'line' empty.
        return;
    }

    if (eof() == true) {
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


void FileReader::seekFromCur(
    const int64_t offset)
{
    seek(offset, SEEK_CUR);
}


void FileReader::seekFromEnd(
    const int64_t offset)
{
    seek(offset, SEEK_END);
}


void FileReader::seekFromSet(
    const int64_t offset)
{
    seek(offset, SEEK_SET);
}


size_t FileReader::size(void) const
{
    return m_fsize;
}


int64_t FileReader::tell(void) const
{
    int64_t offset = static_cast<int64_t>(ftell(m_fp));

    if (offset == -1) {
        throwRuntimeError("ftell failed");
    }

    return offset;
}


void FileReader::seek(
    const int64_t offset,
    const int whence)
{
    if (offset > LONG_MAX) {
        throwRuntimeError("position out of range");
    }

    int rc = fseek(m_fp, offset, whence);
    if (rc != 0) {
        throwRuntimeError("fseek failed");
    }
}


}  // namespace input
}  // namespace dsg
