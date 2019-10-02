#ifndef GENIE_READ_DECODER_H
#define GENIE_READ_DECODER_H

#include <vector>
#include <string>

#include "stream-container.h"
#include <memory>
#include <util/sam-record.h>

namespace lae {
    class LocalAssemblyReadDecoder {
        uint64_t posCounter;

        uint64_t pop(std::vector<uint64_t> *vec);

        std::string compressCigar(const std::string &cigar);

        void decodeVariants(std::string *read, std::string *cigar);

        std::unique_ptr<StreamContainer> container;
    public:

        explicit LocalAssemblyReadDecoder(std::unique_ptr<StreamContainer> _container);

        void decodeRead(const std::string& ref, util::SamRecord* s);

        uint32_t lengthOfNextRead();

        uint32_t offsetOfNextRead();
    };
}


#endif //GENIE_READ_DECODER_H
