#include "sam-writer.h"

namespace genie {
namespace sam {
    SamWriter::SamWriter(sam::SamFileHeader&& fileHeader, std::ostream& stream) : header(std::move(fileHeader)), file(stream) {
        header.print(file);
    }
    const sam::SamFileHeader& SamWriter::getHeader() const { return header; }

    void SamWriter::write(sam::SamRecord&& rec) {
        sam::SamRecord v = std::move(rec);
        auto str = v.toString();
        str += '\n';
        file.write(str.data(), str.length());
    }
}  // namespace sam
}  // namespace genie