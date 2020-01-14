#include "sam-reader.h"

namespace genie {
namespace sam {

SamReader::SamReader(std::istream& _stream) : stream(_stream), header(stream), rec_saved(false) {}

const SamFileHeader& SamReader::getHeader() const { return header; }

void SamReader::read(size_t num, std::vector<SamRecord>& vec) {
    std::string string;
    vec.clear();
    if (rec_saved) {
        rec_saved = false;
        vec.emplace_back(std::move(save));
    }
    while (num && std::getline(stream, string)) {
        vec.emplace_back(string);
        if (vec.front().getRname() != vec.back().getRname()) {
            save = std::move(vec.back());
            vec.pop_back();
            rec_saved = true;
            return;
        }
        num--;
    }
}

bool SamReader::isEnd() { return stream.eof(); }
}  // namespace sam
}  // namespace genie