#ifndef GENIE_FILE_READER_H_
#define GENIE_FILE_READER_H_


#include <string>


namespace genie {


class FileReader {
public:
    FileReader();
    explicit FileReader(const std::string& path);
    virtual ~FileReader();
//    void advance(int64_t offset);
    bool eof() const;
    void close();
    void* handle() const;
    void open(const std::string &path);
    void readLine(std::string* line);
//    void seekFromCur(int64_t offset);
    void seekFromEnd(int64_t offset);
    void seekFromSet(int64_t offset);
    int64_t size() const;
    int64_t tell() const;
protected:
    FILE* m_fp;
    int64_t m_fsize;
private:
    void seek(int64_t offset, int whence);

private:
    static const size_t MAX_LINE_LENGTH = sizeof(char) * (4 * 1024);
    char* m_line;
};


}  // namespace genie


#endif  // GENIE_FILE_READER_H_
