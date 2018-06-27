// Copyright 2018 The genie authors


/**
 *  @file FileReader.h
 *  @brief File input interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_INPUT_FILEREADER_H_
#define DSG_INPUT_FILEREADER_H_


#include <string>

#include "common/constants.h"


namespace dsg {
namespace input {


class FileReader {
 public:
    FileReader(void);

    FileReader(
        const std::string& path);

    virtual ~FileReader(void);

    void advance(
        const int64_t offset);

    bool eof(void) const;

    void close(void);

    void * handle(void) const;

    void open(
        const std::string &path);

    void readLine(
        std::string * const line);

    void seekFromCur(
        const int64_t offset);

    void seekFromEnd(
        const int64_t offset);

    void seekFromSet(
        const int64_t offset);

    size_t size(void) const;

    int64_t tell(void) const;


 protected:
    FILE *m_fp;

    size_t m_fsize;

 private:
    void seek(
        const int64_t offset,
        const int whence);

 private:
    static const size_t MAX_LINE_LENGTH = sizeof(char) * (4 * KB);

    char *m_line;
};


}  // namespace input
}  // namespace dsg


#endif  // DSG_INPUT_FILEREADER_H_
