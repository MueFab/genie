#ifndef GENIE_GABAC_COMPRESSOR_H
#define GENIE_GABAC_COMPRESSOR_H

#include "mpegg-compressor.h"

class GabacCompressor : public MpeggCompressor {
public:
    void flowIn(std::unique_ptr<MpeggRawAu> t, size_t id) override;
    void dryIn() override;
};


#endif //GENIE_GABAC_COMPRESSOR_H
