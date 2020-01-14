#ifndef GENIE_REFERENCE_BASED_ENCODER_H
#define GENIE_REFERENCE_BASED_ENCODER_H

#include "mpegg-encoder.h"

namespace coding {

class ReferenceBasedEncoder : public MpeggEncoder {
   public:
    std::unique_ptr<MpeggRawAu> encode(std::vector<format::mpegg_rec::MpeggRecord>* vec);
};

}  // namespace coding

#endif  // GENIE_REFERENCE_BASED_ENCODER_H
