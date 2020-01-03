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
    SamRecord save;
    bool rec_saved;
   public:
    explicit SamReader(std::istream& _stream) : stream(_stream), header(stream), rec_saved(false) {}

    const SamFileHeader& getHeader() const { return header; }

    void read(size_t num, std::vector<SamRecord>& vec) {
        std::string string;
        vec.clear();
        if(rec_saved) {
            rec_saved = false;
            vec.emplace_back(std::move(save));
        }
        while (num && std::getline(stream, string)) {
            vec.emplace_back(string);
            if(vec.front().getRname() != vec.back().getRname()) {
                save = std::move(vec.back());
                vec.pop_back();
                rec_saved = true;
                return;
            }
            num--;
        }
    }

    bool isEnd() {
        return stream.eof();
    }
};
}  // namespace sam
}  // namespace format

#endif  // GENIE_SAM_READER_H
