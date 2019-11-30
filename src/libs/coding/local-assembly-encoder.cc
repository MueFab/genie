#include "local-assembly-encoder.h"

#include <util/make_unique.h>

#include "full-local-assembly-encoder.h"

#include <format/mpegg_rec/segment.h>

void LocalAssemblyEncoder::flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) {
    // Calculate raw access unit streams from t
    lae::FullLocalAssemblyEncoder encoder(cr_buf_max_size, false);

    for(const auto& r : *t) {
        if(r->getNumberOfRecords() > 1) {
            encoder.add(r.get());
        }
    }

    auto au = util::make_unique<MpeggRawAu>();
    flowOut(std::move(au), id);
}
void LocalAssemblyEncoder::dryIn() {
    // Output data left over
    dryOut();
}

LocalAssemblyEncoder::LocalAssemblyEncoder(uint32_t _cr_buf_max_size) : cr_buf_max_size(_cr_buf_max_size) {

}