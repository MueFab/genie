#ifndef GENIE_LOCAL_ASSEMBLY_DECODER_H
#define GENIE_LOCAL_ASSEMBLY_DECODER_H

#include "mpegg-decoder.h"

class LocalAssemblyDecoder : public MpeggDecoder {
public:
    void decode(std::unique_ptr<BlockPayload> payload, std::vector<format::mpegg_rec::MpeggRecord>* vec);
};


#endif //GENIE_LOCAL_ASSEMBLY_DECODER_H
