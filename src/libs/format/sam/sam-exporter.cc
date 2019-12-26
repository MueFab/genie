#include "sam-exporter.h"
#include <util/ordered-section.h>

void SamExporter::flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) {
    OrderedSection section(&lock, id);
}

void SamExporter::dryIn() {

}

std::string SamExporter::convertECigar2Cigar(const std::string &cigar) {
    static const std::array<char, 256> lut =
            []() -> std::array<char, 256> {
                std::array<char, 256> ret{};
                ret['='] = '=';
                ret['A'] = 'X';
                ret['C'] = 'X';
                ret['G'] = 'X';
                ret['T'] = 'X';
                ret['R'] = 'X';
                ret['Y'] = 'X';
                ret['S'] = 'X';
                ret['W'] = 'X';
                ret['K'] = 'X';
                ret['M'] = 'X';
                ret['B'] = 'X';
                ret['D'] = 'X';
                ret['H'] = 'X';
                ret['V'] = 'X';
                ret['N'] = 'X';
                ret['+'] = 'I';
                ret['-'] = 'D';
                ret['*'] = 'N';
                ret[')'] = 'S';
                ret[']'] = 'H';
                for (char c = '0'; c <= '9'; ++c) {
                    ret[c] = c;
                }
                return ret;
            }();
    std::string ret;
    ret.reserve(cigar.length());
    for (size_t pos = 0; pos < cigar.length(); ++pos) {
        auto c = cigar[pos];
        if (c == '(' || c == '[') {
            continue;
        }
        auto transformed = lut[c];
        if (c == '-' && (pos == 0 || cigar[pos - 1] < '0' ||
                         cigar[pos - 1] > '9')) { // Recognize alphabet character "-" by the missing number in front
            transformed = 'X';
        } else if (transformed == 0) {
            UTILS_DIE("Invalid eCIGAR character or splice which is not supported by CIGAR");
        }
        ret.push_back(transformed);
    }
    return ret;
}
