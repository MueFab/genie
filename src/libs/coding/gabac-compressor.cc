#include "gabac-compressor.h"
#include <util/make_unique.h>

void GabacCompressor::flowIn(std::unique_ptr<MpeggRawAu> t, size_t id) {
    // compress and create block payload
    auto block = util::make_unique<BlockPayload>();
    flowOut(std::move(block), id);
}
void GabacCompressor::dryIn() {
    // Not used
    dryOut();
}