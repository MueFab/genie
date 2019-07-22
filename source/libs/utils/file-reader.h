#ifndef FIO_FILE_READER_H_
#define FIO_FILE_READER_H_

#include <string>

#include "constants.h"

namespace utils {

    class FileReader {
    public:
        FileReader();

        FileReader(const std::string &path);

        virtual ~FileReader();

        void advance(int64_t offset);

        bool eof() const;

        void close();

        void *handle() const;

        void open(const std::string &path);

        void readLine(std::string *line);

        void seekFromCur(int64_t offset);

        void seekFromEnd(int64_t offset);

        void seekFromSet(int64_t offset);

        size_t size() const;

        int64_t tell() const;

    protected:
        FILE *fp_;
        size_t fsize_;

    private:
        void seek(int64_t offset, int whence);

    private:
        static const size_t MAX_LINE_LENGTH = sizeof(char) * (4 * KB);
        char *line_;
    };

}  // namespace utils

#endif  // FIO_FILE_READER_H_
