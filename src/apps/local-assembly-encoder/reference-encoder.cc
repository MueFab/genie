#include "reference-encoder.h"
#include "util/exceptions.h"
#include <map>
#include <limits>

namespace lae {

    LocalAssemblyReferenceEncoder::LocalAssemblyReferenceEncoder(uint32_t _cr_buf_max_size) : cr_buf_max_size(_cr_buf_max_size),
                                                                                              crBufSize(0){

    }

    std::string LocalAssemblyReferenceEncoder::preprocess(const std::string &read, const std::string &cigar) {
        std::string result;
        size_t count = 0;
        size_t read_pos = 0;

        for (size_t cigar_pos = 0; cigar_pos < cigar.length(); ++cigar_pos) {
            if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
                count *= 10;
                count += cigar[cigar_pos] - '0';
                continue;
            }
            switch (cigar[cigar_pos]) {
                case 'M':
                case '=':
                case 'X':
                    for (size_t i = 0; i < count; ++i) {
                        if (read_pos >= read.length()) {
                            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                        }
                        result += read[read_pos++];
                    }
                    break;

                case 'I':
                case 'S':
                case 'P':
                    read_pos += count;
                    break;

                case 'N':
                case 'D':
                case 'H':
                    for (size_t i = 0; i < count; ++i) {
                        result += '0';
                    }
                    break;
                default:
                    UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
            }
            count = 0;
        }

        if (read_pos != read.length()) {
            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }
        return result;
    }

    void LocalAssemblyReferenceEncoder::addRead(const util::SamRecord& s) {
        sequence_positions.push_back(s.pos);
        std::string read = preprocess(s.seq, s.cigar);
        sequences.push_back(read);
        crBufSize += read.length();

        while (crBufSize > cr_buf_max_size) {
            if (sequences.size() == 1) {
                UTILS_THROW_RUNTIME_EXCEPTION("Read too long for current cr_buf_max_size");
            }
            // Erase oldest read
            crBufSize -= sequences.front().length();
            sequences.erase(sequences.begin());
            sequence_positions.erase(sequence_positions.begin());
        }
    }

    std::string LocalAssemblyReferenceEncoder::generateRef(uint32_t offset, uint32_t len) {
        std::string ref;
        for(size_t i = offset; i < offset + len; ++i) {
            ref += majorityVote(i);
        }
        return ref;
    }

    uint32_t LocalAssemblyReferenceEncoder::lengthFromCigar(const std::string& cigar) {
        uint32_t len = 0;
        uint32_t count = 0;
        for (size_t cigar_pos = 0; cigar_pos < cigar.length(); ++cigar_pos) {
            if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
                count *= 10;
                count += cigar[cigar_pos] - '0';
                continue;
            }
            switch (cigar[cigar_pos]) {
                case 'M':
                case '=':
                case 'X':
                case 'D':
                case 'N':
                    len += count;
                    count = 0;
                    break;

                case 'I':
                case 'S':
                case 'P':
                case 'H':
                    count = 0;
                    break;
                default:
                    UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
            }
            count = 0;
        }
        return len;
    }

    std::string LocalAssemblyReferenceEncoder::getReference(uint32_t abs_pos, const std::string &cigar) {
        return generateRef(abs_pos, lengthFromCigar(cigar));
    }

    std::string LocalAssemblyReferenceEncoder::getReference(uint32_t abs_pos, uint32_t len) {
        return generateRef(abs_pos, len);
    }

    char LocalAssemblyReferenceEncoder::majorityVote(uint32_t abs_position) {
        std::map<char, uint16_t> votes;

        // Collect all alignments
        for (size_t i = 0; i < sequences.size(); ++i) {
            int64_t distance = abs_position - sequence_positions[i];
            if(distance >= 0 && uint64_t(distance) < sequences[i].length()){
                char c = sequences[i][distance];
                if (c != '0') {
                    votes[c]++;
                }
            }
        }

        // Find max
        char max = '\0';
        uint16_t max_value = 0;
        for (auto &v : votes) {
            if (max_value < v.second) {
                max_value = v.second;
                max = v.first;
            } else if (max_value == v.second) {
                max = std::min(max, v.first);
            }
        }

        return max;
    }

    void LocalAssemblyReferenceEncoder::printWindow() {
        uint64_t minPos = getWindowBorder();

        for (size_t i = 0; i < sequences.size(); ++i) {
            uint64_t totalOffset = sequence_positions[i] -minPos;
            for (size_t s = 0; s < totalOffset; ++s) {
                std::cout << ".";
            }
            std::cout << sequences[i] << std::endl;
        }
    }

}