#ifndef GENIE_FULL_LOCAL_ASSEMBLY_DECODER_H
#define GENIE_FULL_LOCAL_ASSEMBLY_DECODER_H

#include "read-decoder.h"

extern "C" {
#include "reference-encoder-plainc.h"
}

namespace lae {
    class FullLocalAssemblyDecoder {
    private:
        uint32_t abs_pos;
        LOCAL_ASSEMBLY_STATE* state;
        LocalAssemblyReadDecoder lrd;
        bool debug;
    public:
        FullLocalAssemblyDecoder(std::unique_ptr<StreamContainer> container, uint32_t _cr_buf_size, bool _debug);
        ~FullLocalAssemblyDecoder();
        void decode(util::SamRecord *s);
        void decodePair(util::SamRecord *s, util::SamRecord *s2);
    };
}

#endif //GENIE_FULL_LOCAL_ASSEMBLY_DECODER_H
