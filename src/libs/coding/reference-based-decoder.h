#ifndef GENIE_REFERENCE_BASED_DECODER_H
#define GENIE_REFERENCE_BASED_DECODER_H


#include "mpegg-decoder.h"

class ReferenceBasedDecoder : public MpeggDecoder {
public:
    void decode(std::unique_ptr<BlockPayload> payload, std::vector<format::mpegg_rec::MpeggRecord>* vec);
};


#endif //GENIE_REFERENCE_BASED_DECODER_H
