#ifndef GENIE_LOCAL_ASSEMBLY_ENCODER_H
#define GENIE_LOCAL_ASSEMBLY_ENCODER_H

#include "mpegg-encoder.h"

class LocalAssemblyEncoder : public MpeggEncoder{
private:
    uint32_t cr_buf_max_size;
public:
    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) override;
    void dryIn() override;

    LocalAssemblyEncoder(uint32_t cr_buf_max_size);
};


#endif //GENIE_LOCAL_ASSEMBLY_ENCODER_H
