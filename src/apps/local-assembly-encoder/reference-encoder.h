#ifndef GENIE_REFERENCE_ENCODER_H
#define GENIE_REFERENCE_ENCODER_H

#include <string>
#include <vector>

namespace lae {

    class LocalAssemblyReferenceEncoder {
    public:
        uint32_t cr_buf_max_size;

        std::vector <std::string> sequences;
        std::vector <uint64_t> offsets;
        uint32_t lastOffsetVoted;
        uint32_t crBufSize;
        uint32_t offsetSum;


        // Descriptors
        std::string ref;

        void generateRefToOffset(uint32_t off);

        char majorityVote(uint32_t offset_to_first);

        std::string preprocess(const std::string &read, const std::string &cigar);

    public:
        explicit LocalAssemblyReferenceEncoder(uint32_t _cr_buf_max_size);

        void addRead(const std::string &read_raw, const std::string &cigar, uint32_t pos_offset);

        void finish(std::string *str);

        void printWindow();
    };
}

#endif //GENIE_REFERENCE_ENCODER_H
