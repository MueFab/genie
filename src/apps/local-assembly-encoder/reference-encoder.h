#ifndef GENIE_REFERENCE_ENCODER_H
#define GENIE_REFERENCE_ENCODER_H

#include <string>
#include <vector>
#include <util/sam-record.h>

namespace lae {

    class LocalAssemblyReferenceEncoder {
    public:
        uint32_t cr_buf_max_size;

        std::vector <std::string> sequences;
        std::vector <uint64_t> offsets;
        uint32_t crBufSize;
        uint32_t windowLength;
        uint32_t lastReadPos;

        uint32_t lastSamPos;

        std::string generateRef(uint32_t offset, uint32_t len);

        char majorityVote(uint32_t offset_to_first);

        std::string preprocess(const std::string &read, const std::string &cigar);

        uint32_t lengthFromCigar(const std::string& cigar);

    public:
        explicit LocalAssemblyReferenceEncoder(uint32_t _cr_buf_max_size);

        void addRead(const util::SamRecord& rec);

        std::string getReference(uint32_t pos_offset, const std::string &cigar);

        std::string getReference(uint32_t pos_offset, uint32_t len);

        void printWindow();
    };
}

#endif //GENIE_REFERENCE_ENCODER_H
