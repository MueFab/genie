#ifndef GENIE_READ_DECODER_H
#define GENIE_READ_DECODER_H

#include <vector>
#include <string>

#include "stream-container.h"
#include <memory>

namespace lae {
    class LocalAssemblyReadDecoder {
        const std::string *ref;
        uint64_t posCounter;

        uint64_t pop(std::vector<uint64_t> *vec);

        std::string compressCigar(const std::string &cigar);

        void decodeVariants(std::string *read, std::string *cigar);

        std::unique_ptr<StreamContainer> container;
    public:

        explicit LocalAssemblyReadDecoder(const std::string *_ref, std::unique_ptr<StreamContainer> _container);

        void decodeRead(std::string *read, std::string *cigar, uint64_t *ref_offset);
    };
}


#endif //GENIE_READ_DECODER_H
