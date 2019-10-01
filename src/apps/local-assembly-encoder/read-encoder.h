#ifndef GENIE_READ_ENCODER_H
#define GENIE_READ_ENCODER_H

#include <memory>
#include <string>
#include <vector>
#include <util/sam-record.h>
#include "stream-container.h"

namespace lae {
    class LocalAssemblyReadEncoder {
    private:
        std::unique_ptr<StreamContainer> container;
        uint32_t pos;
        void codeVariants(const std::string &read, const std::string &cigar, const std::string& ref);
        uint64_t pop(std::vector<uint64_t>* vec);
    public:
        explicit LocalAssemblyReadEncoder();

        void addRead(const util::SamRecord& rec, const std::string& ref);

        std::unique_ptr<StreamContainer> pollStreams();
    };
}

#endif //GENIE_READ_ENCODER_H
