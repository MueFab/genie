#ifndef GENIE_SAM_WRITER_H
#define GENIE_SAM_WRITER_H

#include "sam-header.h"
#include "sam-record.h"
class SamWriter {
   private:
    format::sam::SamFileHeader header;
    std::ostream& file;

   public:
    SamWriter(format::sam::SamFileHeader&& fileHeader, std::ostream& stream)
        : header(std::move(fileHeader)), file(stream) {
        header.print(file);
    }
    const format::sam::SamFileHeader& getHeader() const { return header; }

    void write(format::sam::SamRecord&& rec) {
        format::sam::SamRecord v = std::move(rec);
        auto str = v.toString();
        str += '\n';
        file.write(str.data(), str.length());
    }
};

#endif  // GENIE_SAM_WRITER_H
