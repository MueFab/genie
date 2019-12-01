#ifndef GENIE_LOCAL_ASSEMBLY_ENCODER_H
#define GENIE_LOCAL_ASSEMBLY_ENCODER_H

#include "mpegg-encoder.h"
#include "reference-encoder.h"
#include "read-encoder.h"

class LocalAssemblyEncoder : public MpeggEncoder{
private:
    lae::LocalAssemblyReferenceEncoder refCoder;
    lae::LocalAssemblyReadEncoder readCoder;
    bool debug;
    uint32_t cr_buf_max_size;
public:
    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) override;
    void dryIn() override;

    LocalAssemblyEncoder(uint32_t _cr_buf_max_size, bool _debug);
};


#endif //GENIE_LOCAL_ASSEMBLY_ENCODER_H
