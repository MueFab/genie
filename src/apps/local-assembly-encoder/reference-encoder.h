#ifndef GENIE_REFERENCE_ENCODER_H
#define GENIE_REFERENCE_ENCODER_H

#include <string>
#include <vector>
#include <util/sam-record.h>
#include <limits>

namespace lae {

    class LocalAssemblyReferenceEncoder {
    public:
        uint32_t cr_buf_max_size;

        std::vector <std::string> sequences;
        std::vector <uint64_t> sequence_positions;
        uint32_t crBufSize;

        std::string generateRef(uint32_t offset, uint32_t len);

        char majorityVote(uint32_t offset_to_first);

        std::string preprocess(const std::string &read, const std::string &cigar);

        uint32_t lengthFromCigar(const std::string& cigar);

    public:
        explicit LocalAssemblyReferenceEncoder(uint32_t _cr_buf_max_size);

        void addRead(const util::SamRecord& rec);

        std::string getReference(uint32_t pos_offset, const std::string &cigar);

        std::string getReference(uint32_t pos_offset, uint32_t len);

        uint64_t getWindowBorder() {
            uint64_t minPos = std::numeric_limits<uint64_t >::max();
            for(auto &p : sequence_positions) {
                minPos = std::min(minPos, p);
            }
            return minPos;
        }

        void printWindow();
    };
}

#endif //GENIE_REFERENCE_ENCODER_H