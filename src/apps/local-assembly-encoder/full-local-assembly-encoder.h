#ifndef GENIE_FULL_LOCAL_ASSEMBLY_ENCODER_H
#define GENIE_FULL_LOCAL_ASSEMBLY_ENCODER_H

#include "reference-encoder.h"
#include "read-encoder.h"

namespace lae {
    class FullLocalAssemblyEncoder {
    private:
        LocalAssemblyReferenceEncoder refCoder;
        LocalAssemblyReadEncoder readCoder;
        bool debug;
    public:
        explicit FullLocalAssemblyEncoder(uint32_t cr_buf_max_size, bool _debug);

        void add(const util::SamRecord &rec);
        void addPair(const util::SamRecord &rec1, const util::SamRecord &rec2);

        std::unique_ptr<StreamContainer> pollStreams();
    };
}

#endif //GENIE_FULL_LOCAL_ASSEMBLY_ENCODER_H

