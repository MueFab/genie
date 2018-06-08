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


namespace dsg {


class FileReader {
 public:
    FileReader(void);
    FileReader(const std::string& path);
    virtual ~FileReader(void);

    void open(const std::string &path);
    void close(void);

    void advance(const size_t offset);
    bool eof(void) const;
    void * handle(void) const;
    void seek(const size_t pos);
    size_t size(void) const;
    size_t tell(void) const;

 protected:
    FILE *fp_;
    size_t fsize_;
    bool isOpen_;
};


}  // namespace dsg


#endif  // DSG_INPUT_FILEREADER_H_
