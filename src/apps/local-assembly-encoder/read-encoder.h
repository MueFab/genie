#ifndef GENIE_READ_ENCODER_H
#define GENIE_READ_ENCODER_H

#include <memory>
#include <string>
#include <vector>
#include "stream-container.h"

namespace lae {
    class LocalAssemblyReadEncoder {
    private:
        std::string *ref;
        uint64_t posCounter;

        std::unique_ptr<StreamContainer> container;
        uint64_t codeVariants(const std::string &read, const std::string &cigar, uint64_t ref_offset);
        uint64_t pop(std::vector<uint64_t>* vec);
    public:
        explicit LocalAssemblyReadEncoder(std::string *_ref);

        void addRead(const std::string &read_raw, const std::string &cigar, uint32_t pos_offset);

        std::unique_ptr<StreamContainer> pollStreams();
    };
}

#endif //GENIE_READ_ENCODER_H
