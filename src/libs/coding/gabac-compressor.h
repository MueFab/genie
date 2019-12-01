#ifndef GENIE_GABAC_COMPRESSOR_H
#define GENIE_GABAC_COMPRESSOR_H

#include "mpegg-compressor.h"
#include <gabac/gabac.h>

#include "genie-gabac-output-stream.h"
#include <util/make_unique.h>

class GabacSeqConfSet {
    typedef std::vector<gabac::EncodingConfiguration> SeqConf;
    std::vector<SeqConf> conf;

public:
    GabacSeqConfSet();

    const gabac::EncodingConfiguration* getConf(MpeggRawAu::GenomicDescriptor desc, size_t sub);


};

class GabacCompressor : public MpeggCompressor {
private:
    void compress(const gabac::EncodingConfiguration& conf, gabac::DataBlock* in, std::vector<gabac::DataBlock>* out);
public:
    GabacSeqConfSet configSet;

    void flowIn(std::unique_ptr<MpeggRawAu> t, size_t id) override;
    void dryIn() override;
};


#endif //GENIE_GABAC_COMPRESSOR_H
