#ifndef GENIE_SPRING_DECODER_H
#define GENIE_SPRING_DECODER_H


#include "mpegg-decoder.h"

class SpringDecoder : public MpeggDecoder {
public:
    void decode(std::unique_ptr<BlockPayload> payload, std::vector<format::mpegg_rec::MpeggRecord>* vec);
};


#endif //GENIE_SPRING_DECODER_H
