#ifndef GENIE_FULL_LOCAL_ASSEMBLY_DECODER_H_
#define GENIE_FULL_LOCAL_ASSEMBLY_DECODER_H_

#include "read-decoder.h"

/*namespace lae {
    class FullLocalAssemblyDecoder {
    private:
        uint32_t mapping_pos;
        LOCAL_ASSEMBLY_STATE* state;
        LocalAssemblyReadDecoder lrd;
        bool debug;
    public:
        FullLocalAssemblyDecoder(std::unique_ptr<StreamContainer> container, uint32_t cr_buf_size, bool _debug);
        ~FullLocalAssemblyDecoder();
        void decode(util::SamRecord *mpegg_record);
        void decodePair(util::SamRecord *mpegg_record_segment_1, util::SamRecord *mpegg_record_segment_2);
    private:
        void init(uint32_t cr_buf_max_size);
    };
}*/

#endif //GENIE_FULL_LOCAL_ASSEMBLY_DECODER_H_
