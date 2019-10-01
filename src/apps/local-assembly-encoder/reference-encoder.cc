#include "reference-encoder.h"
#include "exceptions.h"
#include <map>

namespace lae {

    LocalAssemblyReferenceEncoder::LocalAssemblyReferenceEncoder(uint32_t _cr_buf_max_size) : cr_buf_max_size(
            _cr_buf_max_size),
                                                                                              lastOffsetVoted(0),
                                                                                              crBufSize(0),
                                                                                              offsetSum(0) {

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

    void
    LocalAssemblyReferenceEncoder::addRead(const std::string &read_raw, const std::string &cigar, uint32_t pos_offset) {
        std::string read = preprocess(read_raw, cigar);
        if (sequences.empty()) {
            sequences.push_back(read);
            crBufSize = read.length();

            return;
        }

        generateRefToOffset(offsetSum + pos_offset);

        sequences.push_back(read);
        crBufSize += read.length();
        offsetSum += pos_offset;
        offsets.push_back(pos_offset);

        while (crBufSize > cr_buf_max_size) {
            if (sequences.size() == 1) {
                GENIE_THROW_RUNTIME_EXCEPTION("Read too long for current cr_buf_max_size");
            }
            // Update to new reference position of first read
            offsetSum -= offsets.front();
            lastOffsetVoted -= offsets.front();

            // Erase oldest read
            crBufSize -= sequences.front().length();
            sequences.erase(sequences.begin());
            offsets.erase(offsets.begin());
        }
    }

    void LocalAssemblyReferenceEncoder::generateRefToOffset(uint32_t off) {
        while (lastOffsetVoted < off) {
            ref += majorityVote(lastOffsetVoted);
            lastOffsetVoted++;
        }
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
        char max = 'N';
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

    void LocalAssemblyReferenceEncoder::finish(std::string *str) {
        uint32_t totalOffset = 0;
        for (const auto &a : offsets) {
            totalOffset += a;
        }
        totalOffset += sequences.back().length();
        generateRefToOffset(totalOffset);
        offsets.clear();
        sequences.clear();
        str->clear();
        str->swap(ref);
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