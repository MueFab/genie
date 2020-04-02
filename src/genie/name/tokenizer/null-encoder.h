#ifndef GENIE_NULL_ENCODER_H
#define GENIE_NULL_ENCODER_H

namespace genie {
namespace name {
namespace tokenizer {
class NullEncoder : public core::NameEncoder {
   public:
    core::AccessUnitRaw::Descriptor encode(const core::record::Chunk& recs) override {
        core::AccessUnitRaw::Descriptor ret(core::GenDesc::RNAME);
        return ret;
    }
};
}
}
}

#endif  // GENIE_NULL_ENCODER_H
