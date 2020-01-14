#ifndef GENIE_LOCAL_ASSEMBLY_DECODER_H
#define GENIE_LOCAL_ASSEMBLY_DECODER_H

#include "backbone/mpegg-decoder.h"
#include "local_assembly/reference-encoder.h"
#include "mpegg_read/read-decoder.h"

namespace genie {
namespace local_assembly {

class LocalAssemblyDecoder : public genie::MpeggDecoder {
   private:
   public:
    void flowIn(genie::MpeggRawAu&& t, size_t id) override {
        size_t numRecords = t.getNumRecords();
        size_t bufSize = t.getParameters().getParameterSetCrps().getCrpsInfo().getBufMaxSize();
        size_t segments = t.getParameters().getNumberTemplateSegments();
        uint16_t ref = t.getReference();
        LocalAssemblyReferenceEncoder refEncoder(bufSize);
        mpegg_read::LocalAssemblyReadDecoder decoder(std::move(t), segments);
        genie::mpegg_rec::MpeggChunk chunk;
        for (size_t recID = 0; recID < numRecords; ++recID) {
            auto meta = decoder.readSegmentMeta();
            std::vector<std::string> refs;
            refs.reserve(meta.size());
            for (const auto& m : meta) {
                refs.emplace_back(refEncoder.getReference(m.position, m.length));
            }
            auto rec = decoder.pull(ref, std::move(refs));
            refEncoder.addRead(rec);
            chunk.emplace_back(std::move(rec));
        }
        flowOut(std::move(chunk), id);
    }
    void dryIn() override { dryOut(); }
};

}  // namespace local_assembly
}  // namespace genie

#endif  // GENIE_LOCAL_ASSEMBLY_DECODER_H
