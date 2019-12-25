#ifndef GENIE_READ_DECODER_H
#define GENIE_READ_DECODER_H

#include <string>
#include <vector>

#include <format/sam/sam-record.h>
#include <memory>
#include "stream-container.h"

namespace lae {
class LocalAssemblyReadDecoder {
    uint64_t posCounter;

    uint64_t pop(gabac::DataBlock* vec);

    std::string compressCigar(const std::string& cigar);

    void decodeVariants(std::string* read, std::string* cigar);

    std::unique_ptr<StreamContainer> container;

   public:
    explicit LocalAssemblyReadDecoder(std::unique_ptr<StreamContainer> _container);

    void decodeRead(const std::string& ref, format::sam::SamRecord* s);
    void decodePair(const std::string& ref1, format::sam::SamRecord* s1, const std::string& ref2,
                    format::sam::SamRecord* s2);

    uint32_t lengthOfNextRead();
    uint32_t offsetOfNextRead();
    uint32_t lengthOfSecondNextRead();
    uint32_t offsetOfSecondNextRead();
};
}  // namespace lae

#endif  // GENIE_READ_DECODER_H
