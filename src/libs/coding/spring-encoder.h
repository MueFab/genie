#ifndef GENIE_SPRING_ENCODER_H
#define GENIE_SPRING_ENCODER_H


#include "mpegg-encoder.h"

class SpringEncoder : public MpeggEncoder{
public:
    std::unique_ptr<MpeggRawAu> encode(std::vector<format::mpegg_rec::MpeggRecord>* vec);
};


#endif //GENIE_SPRING_ENCODER_H
