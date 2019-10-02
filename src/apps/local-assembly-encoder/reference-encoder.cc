#include "reference-encoder.h"
#include "exceptions.h"
#include <map>

namespace lae {

    LocalAssemblyReferenceEncoder::LocalAssemblyReferenceEncoder(uint32_t _cr_buf_max_size) : cr_buf_max_size(_cr_buf_max_size),
                                                                                              crBufSize(0),
                                                                                              windowLength(0),
                                                                                              lastReadPos(0){

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
                            GENIE_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
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
                    GENIE_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
            }
            count = 0;
        }

        if (read_pos != read.length()) {
            GENIE_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }
        return result;
    }

    void LocalAssemblyReferenceEncoder::addRead(const util::SamRecord& s) {
        uint32_t pos_offset = s.pos - lastSamPos;
        lastSamPos = s.pos;

        std::string read = preprocess(s.seq, s.cigar);

        if(!sequences.empty()) {
            offsets.push_back(pos_offset);
            lastReadPos += pos_offset;
        }

        if(lastReadPos + read.length() > windowLength) {
            windowLength = lastReadPos + read.length();
        }

        sequences.push_back(read);
        crBufSize += read.length();


        while (crBufSize > cr_buf_max_size) {
            if (sequences.size() == 1) {
                GENIE_THROW_RUNTIME_EXCEPTION("Read too long for current cr_buf_max_size");
            }

            lastReadPos -= offsets.front();
            windowLength -= offsets.front();

            // Erase oldest read
            crBufSize -= sequences.front().length();
            sequences.erase(sequences.begin());
            offsets.erase(offsets.begin());
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
                    GENIE_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
            }
            count = 0;
        }
        return len;
    }

    std::string LocalAssemblyReferenceEncoder::getReference(uint32_t abs_pos, const std::string &cigar) {
        uint32_t pos = lastReadPos;
        if(!sequences.empty()) {
            pos += abs_pos - lastSamPos;
        }
        return generateRef(pos, lengthFromCigar(cigar));
    }

    std::string LocalAssemblyReferenceEncoder::getReference(uint32_t abs_pos, uint32_t len) {
        uint32_t pos = lastReadPos;
        if(!sequences.empty()) {
            pos += abs_pos - lastSamPos;
        }
        return generateRef(pos, len);
    }

    char LocalAssemblyReferenceEncoder::majorityVote(uint32_t offset_to_first) {
        std::map<char, uint16_t> votes;
        uint32_t cur_read_off = 0;

        // Collect all alignments
        for (size_t i = 0; i < sequences.size(); ++i) {
            if (offset_to_first - cur_read_off < sequences[i].length()) {
                char c = sequences[i][offset_to_first - cur_read_off];
                if (c != '0') {
                    votes[c]++;
                }
            }
            if (i != sequences.size() - 1) {
                cur_read_off += offsets[i];
            }
            if (cur_read_off > offset_to_first) {
                break;
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
        uint32_t totalOffset = 0;
        for (size_t i = 0; i < sequences.size(); ++i) {
            if (i > 0) {
                totalOffset += offsets[i - 1];
            }
            for (size_t s = 0; s < totalOffset; ++s) {
                std::cout << ".";
            }
            std::cout << sequences[i] << std::endl;
        }
    }

}