#ifndef GENIE_SAM_READER_H
#define GENIE_SAM_READER_H

#include <istream>
#include "sam-header.h"
#include "sam-record.h"

namespace format {
namespace sam {
class SamReader {
   private:
    std::istream& stream;
    SamFileHeader header;

   public:
    explicit SamReader(std::istream& _stream) : stream(_stream), header(stream) {}

    const SamFileHeader& getHeader() const { return header; }

    void read(size_t num, std::vector<SamRecord>& vec) {
        std::string string;
        vec.clear();
        while (num && std::getline(stream, string)) {
            vec.emplace_back(string);
            num--;
        }
    }
};
}  // namespace sam
}  // namespace format

#endif  // GENIE_SAM_READER_H
